/**
 *
 * @todo Add @copyright line once project copyright wording is finalised.
 */

/**
 * @file StateMachine.h
 * @brief Allocation-free, type-safe finite state machine with Entry/Loop/Exit
 *        member-function-pointer hooks, optional legal-transition matrix, and
 *        always-on visit / dwell instrumentation.
 *
 * This is the canonical mid-tier state machine in `hf-utils-general`. It sits
 * between the bare-bones `SimpleStateMachine` (no per-state hooks) and the
 * flexible `SlightlyAdvancedStateMachine` (per-state `std::function` hooks,
 * heap-prone with fat captures). `StateMachine` keeps the per-state hook model
 * but binds hooks at compile time using member-function pointers on a single
 * owner type, eliminating all heap risk and indirect-call overhead from
 * `std::function`.
 *
 * ### Threading and allocation
 * - **No heap allocation** at any time. All storage is `std::array`.
 * - **Single-owner mutation**: the owner task drives `Update()` and
 *   `RequestTransition()`. External tasks must use `OnExternalIntent()` which
 *   posts to a single-slot atomic inbox (last-writer-wins) drained from the
 *   owner task on the next `Update()`.
 * - The optional owner-task token (`SetOwnerToken`) lets consumers detect
 *   accidental cross-task mutation in debug builds.
 *
 * ### Lifecycle contract
 * 1. Construct with `StateMachine(Owner&, EnumT initial)`.
 * 2. Call `Register(state, StateActions<Owner>{...})` once per enum value.
 * 3. Optionally call `SetTransitionMatrix(...)` to declare legal edges.
 * 4. Call `Finalize()`; verifies every enum slot has at least a Loop fn.
 *    Returns false if any state was missed — owners should propagate that
 *    failure to their `Initialize()` so boot fails loudly.
 * 5. Each tick: call `Update(now_ms)`. The state machine drains any pending
 *    intent, runs `Exit` of the previous state, runs `Entry` of the next
 *    state, then runs `Loop` of the (possibly new) current state. The
 *    return value is the loop-back delay in ms requested by the loop fn.
 *
 * ### Hook signatures (member functions on `Owner`)
 *  - `bool   Owner::Entry() noexcept` — called on entry. Return `true` to
 *    advance into running mode; `false` keeps the machine in entering mode
 *    and the next `Update()` re-attempts entry.
 *  - `uint32_t Owner::Loop()  noexcept` — called every tick while running.
 *    Return value is the requested ms-until-next-tick (purely advisory; the
 *    owner thread chooses whether to honor it).
 *  - `bool   Owner::Exit()  noexcept` — called once on transition.
 *    Returning `false` aborts the transition (machine stays in current
 *    state) and the failure is recorded in the last-transition snapshot.
 *
 * Any hook may be omitted (left as `nullptr`) — that is treated as a no-op
 * success.
 *
 * @note This file has zero RTOS dependencies. Owners pass `now_ms` into
 *       `Update()` so dwell accounting is wall-clock agnostic.
 */

#ifndef HF_UTILS_GENERAL_STATEMACHINE_H_
#define HF_UTILS_GENERAL_STATEMACHINE_H_

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>

namespace hf_utils {

/**
 * @brief Phase of the currently-active state.
 *
 * Reported through `GetCurrentPhase()` and the `LastTransition` snapshot.
 */
enum class StatePhase : uint8_t {
    Entering = 0, ///< Entry fn has not yet returned `true` for this state.
    Running  = 1, ///< Entry succeeded; Loop fn runs each tick.
    Exiting  = 2, ///< Owner asked to leave; Exit fn pending.
};

/**
 * @brief Compile-time bound triplet of Entry/Loop/Exit member-function
 *        pointers on `Owner`.
 *
 * Any field may be `nullptr`, meaning "no-op success" (Entry/Exit) or
 * "no work this tick" (Loop, in which case `kDefaultLoopDelayMs` is
 * returned).
 *
 * @tparam Owner Class type that hosts the hook implementations.
 */
template <typename Owner>
struct StateActions {
    /// `bool Owner::*entry() noexcept` — entry hook.
    using EntryFn = bool     (Owner::*)() noexcept;
    /// `uint32_t Owner::*loop() noexcept` — loop hook.
    using LoopFn  = uint32_t (Owner::*)() noexcept;
    /// `bool Owner::*exit() noexcept` — exit hook.
    using ExitFn  = bool     (Owner::*)() noexcept;

    EntryFn entry{nullptr}; ///< Optional entry hook.
    LoopFn  loop {nullptr}; ///< Optional loop hook.
    ExitFn  exit {nullptr}; ///< Optional exit hook.
};

/**
 * @brief Snapshot of the most recent transition for diagnostics.
 *
 * Populated by `Update()` whenever an intent is drained. Read-only from the
 * outside; values for fields that are unused (e.g. before any transition has
 * occurred) are zero-valued.
 *
 * @tparam EnumT The state enumeration type.
 */
template <typename EnumT>
struct LastTransition {
    EnumT    from{};       ///< State left.
    EnumT    to{};         ///< State entered.
    uint32_t at_ms{0};     ///< `now_ms` value at the moment of transition.
    bool     exit_ok{true};  ///< Exit hook return; `true` if no exit hook.
    bool     entry_ok{true}; ///< Entry hook return at first attempt; `true` if no entry hook.
};

/**
 * @brief Allocation-free finite state machine bound to a single owner type.
 *
 * @tparam Owner The class hosting Entry/Loop/Exit hooks.
 * @tparam EnumT Strongly-typed state enum (`enum class`). Underlying type
 *               must fit in a `uint8_t` — i.e. fewer than 256 states.
 * @tparam N     Number of declared states. Must be ≤ 64 (legal-transition
 *               matrix uses `uint64_t` per row).
 *
 * @see hf_utils::StateActions, hf_utils::LastTransition
 */
template <typename Owner, typename EnumT, std::size_t N>
class StateMachine {
public:
    static_assert(N > 0,        "StateMachine requires at least one state.");
    static_assert(N <= 64,      "StateMachine supports at most 64 states.");

    using Actions = StateActions<Owner>;

    /// Default loop-back interval used when a state has no Loop hook.
    static constexpr uint32_t kDefaultLoopDelayMs = 100U;

    /**
     * @brief Construct a state machine bound to `owner` with `initial` as
     *        the starting state.
     *
     * The initial state begins in the `Entering` phase; its Entry hook (if
     * any) runs on the first `Update()` call.
     *
     * @param owner   Reference to the owner; stored for the lifetime of the
     *                state machine. Owner must outlive the state machine.
     * @param initial Initial state.
     */
    StateMachine(Owner& owner, EnumT initial) noexcept
        : owner_(&owner)
        , current_(initial)
        , previous_(initial)
        , phase_(StatePhase::Entering)
        , entered_at_ms_(0)
    {
        for (auto& m : visits_)         { m = 0; }
        for (auto& m : total_dwell_ms_) { m = 0; }
        for (auto& m : last_dwell_ms_)  { m = 0; }
        for (auto& m : max_dwell_ms_)   { m = 0; }
        for (auto& m : allowed_)        { m = ~uint64_t{0}; } // all edges legal by default
    }

    StateMachine(const StateMachine&)            = delete;
    StateMachine& operator=(const StateMachine&) = delete;
    StateMachine(StateMachine&&)                 = delete;
    StateMachine& operator=(StateMachine&&)      = delete;

    //==============================================================//
    /// REGISTRATION
    //==============================================================//

    /**
     * @brief Register the Entry/Loop/Exit triplet for one state.
     *
     * Call once per enum value during owner construction or `Initialize()`.
     * Re-registration is permitted but logged via the `re_register_count`
     * diagnostic counter — tooling can use that to detect double-registration
     * mistakes.
     *
     * @param s       State to bind.
     * @param actions Triplet of member-function pointers; any field may be
     *                `nullptr`.
     */
    void Register(EnumT s, Actions actions) noexcept
    {
        const auto idx = Index(s);
        if (idx >= N) { return; }
        if (registered_[idx]) { ++re_register_count_; }
        table_[idx]      = actions;
        registered_[idx] = true;
    }

    /**
     * @brief Declare which target states are reachable from each source.
     *
     * `mask[from]` is a bitmap of allowed `to` indices: bit `i` set means
     * "transition from state index `from` to state index `i` is legal."
     * Defaults: all transitions legal.
     *
     * Disallowed `RequestTransition()` / `OnExternalIntent()` calls are
     * counted via `IllegalTransitionCount()` and rejected without taking
     * effect.
     *
     * @param mask Per-source bitmap of allowed targets.
     */
    void SetTransitionMatrix(const std::array<uint64_t, N>& mask) noexcept
    {
        allowed_ = mask;
    }

    /**
     * @brief Optional per-state stuck-state watchdog.
     *
     * If non-zero, the machine compares accumulated dwell each tick against
     * `max_ms`; on breach `max_dwell_breach_count_` increments and
     * `max_dwell_last_breach_state_` is updated. Owners can read the counters
     * each tick and react (e.g. force a fault transition).
     *
     * @param s      State to watch.
     * @param max_ms Maximum allowed dwell in ms (0 disables).
     */
    void SetMaxDwell(EnumT s, uint32_t max_ms) noexcept
    {
        const auto idx = Index(s);
        if (idx < N) { max_dwell_ms_[idx] = max_ms; }
    }

    /**
     * @brief Verify every state has at least a Loop hook (or has been
     *        registered with all hooks `nullptr`, which is treated as an
     *        explicit no-op state).
     *
     * Owners should call `Finalize()` after all `Register()` calls and
     * propagate `false` to a hard initialization failure so unconfigured
     * states never get reached at runtime.
     *
     * @return `true` if every state was registered (regardless of hook
     *         contents); `false` if any state slot is unregistered.
     */
    bool Finalize() noexcept
    {
        for (std::size_t i = 0; i < N; ++i) {
            if (!registered_[i]) { return false; }
        }
        finalized_ = true;
        return true;
    }

    /// @return `true` if `Finalize()` has been called and all states registered.
    bool IsFinalized() const noexcept { return finalized_; }

    /**
     * @brief Reset runtime state to the supplied initial state.
     *
     * Clears phase to `Entering`, drops any pending external/owner intent,
     * zeroes per-state visit / dwell counters, and resets the watchdog
     * latch. Registrations, the transition matrix, dwell limits, the owner
     * token, and the `finalized_` flag are **preserved**.
     *
     * Intended for `BaseThread::Setup()` re-arming after a `ResetVariables()`
     * in a stop-then-start cycle: the thread re-enters `initial` cleanly with
     * a fresh dwell counter.
     *
     * @param initial New starting state.
     */
    void Reset(EnumT initial) noexcept
    {
        current_       = initial;
        previous_      = initial;
        phase_         = StatePhase::Entering;
        entered_at_ms_ = 0;
        intent_pending_           = false;
        intent_target_            = initial;
        pending_intent_.store(0, std::memory_order_release);
        max_dwell_breach_latched_ = false;
        for (auto& m : visits_)         { m = 0; }
        for (auto& m : total_dwell_ms_) { m = 0; }
        for (auto& m : last_dwell_ms_)  { m = 0; }
        last_transition_ = LastTransition<EnumT>{};
    }

    //==============================================================//
    /// OWNERSHIP
    //==============================================================//

    /**
     * @brief Set an opaque owner-task token used to detect accidental
     *        cross-task `RequestTransition()` calls.
     *
     * The state machine itself does not interpret the token; it just compares
     * it against the value passed to `RequestTransition()`. A mismatch
     * increments `cross_task_request_count_` and rejects the request.
     *
     * Pass `0` to disable the check (default).
     *
     * @param token Opaque owner-task identifier.
     */
    void SetOwnerToken(uintptr_t token) noexcept { owner_token_ = token; }

    //==============================================================//
    /// TRANSITIONS
    //==============================================================//

    /**
     * @brief Request the next transition from the owner task.
     *
     * The transition is applied on the next `Update()` call. If a request
     * was already pending it is overwritten — last-writer-wins semantics.
     *
     * @param to            Target state.
     * @param caller_token  Optional caller-task token; when non-zero and not
     *                      equal to the configured owner token, the request
     *                      is rejected and `cross_task_request_count_`
     *                      increments.
     * @return `true` if accepted (legal transition + correct caller token);
     *         `false` if rejected (illegal target or wrong caller).
     */
    bool RequestTransition(EnumT to, uintptr_t caller_token = 0) noexcept
    {
        if (owner_token_ != 0 && caller_token != 0 && caller_token != owner_token_) {
            ++cross_task_request_count_;
            return false;
        }
        return PostIntent(to, /*from_owner=*/true);
    }

    /**
     * @brief Post an external transition intent (any task, lock-free).
     *
     * Stores the intent in a single-slot atomic inbox; drained on the next
     * `Update()` call. If multiple external intents arrive between ticks the
     * latest wins.
     *
     * Illegal targets are counted via `IllegalTransitionCount()` and not
     * applied.
     *
     * @param to Target state.
     */
    void OnExternalIntent(EnumT to) noexcept
    {
        // Encode as (1<<7) | index; 0 means "no pending".
        const auto idx = Index(to);
        if (idx >= N) { ++illegal_count_; last_illegal_to_ = to; return; }
        pending_intent_.store(static_cast<uint8_t>(0x80 | idx), std::memory_order_release);
    }

    //==============================================================//
    /// TICK
    //==============================================================//

    /**
     * @brief Drive one tick: drain any pending intent, run Exit→Entry, run
     *        Loop. Updates dwell, visits, and watchdog counters.
     *
     * Must only be called from the owner task.
     *
     * @param now_ms       Wall-clock millisecond stamp; used for dwell
     *                     accounting and the `LastTransition` snapshot.
     * @param[out] stepped Optional flag set to `true` if a state change
     *                     completed during this call.
     * @return Loop-back delay in ms requested by the (possibly new)
     *         current state's Loop hook, or `kDefaultLoopDelayMs` if none.
     */
    uint32_t Update(uint32_t now_ms, bool* stepped = nullptr) noexcept
    {
        if (stepped) { *stepped = false; }
        bool attempted_entry_this_tick = false;

        // Drain a pending external intent first, if no owner-side request is queued.
        if (!intent_pending_) {
            const uint8_t raw = pending_intent_.exchange(0, std::memory_order_acq_rel);
            if (raw & 0x80) {
                const std::size_t idx = (raw & 0x3F);
                if (idx < N) {
                    const EnumT to = static_cast<EnumT>(idx);
                    if (IsLegal(current_, to)) {
                        intent_target_  = to;
                        intent_pending_ = true;
                    } else {
                        ++illegal_count_;
                        last_illegal_from_ = current_;
                        last_illegal_to_   = to;
                    }
                }
            }
        }

        // If there is a pending transition, do Exit -> swap state -> Entry.
        if (intent_pending_) {
            const EnumT from = current_;
            const EnumT to   = intent_target_;
            phase_ = StatePhase::Exiting;

            bool exit_ok = true;
            const auto exit_fn = table_[Index(from)].exit;
            if (exit_fn != nullptr) {
                exit_ok = (owner_->*exit_fn)();
            }

            if (exit_ok) {
                // Update dwell of the leaving state.
                const uint32_t dwell = now_ms - entered_at_ms_;
                last_dwell_ms_[Index(from)]  = dwell;
                total_dwell_ms_[Index(from)] += dwell;

                previous_      = from;
                current_       = to;
                entered_at_ms_ = now_ms;
                visits_[Index(to)] += 1U;
                phase_         = StatePhase::Entering;
                max_dwell_breach_latched_ = false; // re-arm watchdog for new state

                bool entry_ok = true;
                const auto entry_fn = table_[Index(to)].entry;
                if (entry_fn != nullptr) {
                    entry_ok = (owner_->*entry_fn)();
                    attempted_entry_this_tick = true;
                }
                if (entry_ok) { phase_ = StatePhase::Running; }

                last_transition_ = {from, to, now_ms, exit_ok, entry_ok};
                intent_pending_  = false;
                if (stepped) { *stepped = true; }
            } else {
                // Exit refused: leave intent pending for retry on next tick.
                last_transition_ = {from, to, now_ms, /*exit_ok=*/false, /*entry_ok=*/true};
                phase_ = StatePhase::Running; // back to running; intent retries next tick
            }
        }

        // If still in Entering (entry returned false earlier), re-attempt entry,
        // but only if we haven't already called the entry hook this tick.
        if (phase_ == StatePhase::Entering) {
            if (attempted_entry_this_tick) {
                return kEnteringRetryDelayMs;
            }
            const auto entry_fn = table_[Index(current_)].entry;
            if (entry_fn == nullptr) {
                phase_ = StatePhase::Running;
            } else if ((owner_->*entry_fn)()) {
                phase_ = StatePhase::Running;
            } else {
                // Still entering; return short retry delay.
                return kEnteringRetryDelayMs;
            }
        }

        // Watchdog check (only meaningful while running).
        if (phase_ == StatePhase::Running) {
            const uint32_t dwell_now = now_ms - entered_at_ms_;
            const uint32_t cap       = max_dwell_ms_[Index(current_)];
            if (cap != 0 && dwell_now > cap && !max_dwell_breach_latched_) {
                ++max_dwell_breach_count_;
                max_dwell_last_breach_state_ = current_;
                max_dwell_breach_latched_    = true;
            }
            // Latch resets on transition (handled below).
        }

        // Run Loop of the current state.
        const auto loop_fn = table_[Index(current_)].loop;
        if (loop_fn == nullptr) { return kDefaultLoopDelayMs; }
        return (owner_->*loop_fn)();
    }

    //==============================================================//
    /// QUERIES
    //==============================================================//

    /// @return current state.
    EnumT GetCurrentState() const noexcept { return current_; }

    /// @return previous state (the one we transitioned out of).
    EnumT GetPreviousState() const noexcept { return previous_; }

    /// @return current phase (Entering/Running/Exiting).
    StatePhase GetCurrentPhase() const noexcept { return phase_; }

    /// @return milliseconds since the current state was entered (using last
    ///         tick's `now_ms`); zero before first tick.
    uint32_t DwellMs(uint32_t now_ms) const noexcept { return now_ms - entered_at_ms_; }

    /// @return number of times state `s` has been entered.
    uint32_t VisitCount(EnumT s) const noexcept { return visits_[Index(s)]; }

    /// @return last completed dwell in ms for state `s` (0 if never visited).
    uint32_t LastDwellMs(EnumT s) const noexcept { return last_dwell_ms_[Index(s)]; }

    /// @return cumulative dwell in ms for state `s`.
    uint32_t TotalDwellMs(EnumT s) const noexcept { return total_dwell_ms_[Index(s)]; }

    /// @return total illegal-transition rejections across all sources.
    uint32_t IllegalTransitionCount() const noexcept { return illegal_count_; }

    /// @return state that the last illegal transition targeted.
    EnumT LastIllegalFrom() const noexcept { return last_illegal_from_; }
    EnumT LastIllegalTo()   const noexcept { return last_illegal_to_; }

    /// @return total cross-task `RequestTransition()` rejections.
    uint32_t CrossTaskRequestCount() const noexcept { return cross_task_request_count_; }

    /// @return total `Register()` re-registrations observed.
    uint32_t ReRegisterCount() const noexcept { return re_register_count_; }

    /// @return number of times the per-state max-dwell watchdog has fired.
    uint32_t MaxDwellBreachCount() const noexcept { return max_dwell_breach_count_; }

    /// @return last state that breached its max-dwell watchdog.
    EnumT MaxDwellLastBreachState() const noexcept { return max_dwell_last_breach_state_; }

    /// @return snapshot of the most recent transition.
    LastTransition<EnumT> GetLastTransition() const noexcept { return last_transition_; }

    /// @return `true` if state `to` is legal from state `from` per the
    ///         configured transition matrix.
    bool IsLegal(EnumT from, EnumT to) const noexcept
    {
        const auto fi = Index(from);
        const auto ti = Index(to);
        if (fi >= N || ti >= N) { return false; }
        return (allowed_[fi] & (uint64_t{1} << ti)) != 0;
    }

    /// @return `true` if every state has been registered.
    bool AllStatesRegistered() const noexcept
    {
        for (std::size_t i = 0; i < N; ++i) {
            if (!registered_[i]) { return false; }
        }
        return true;
    }

private:
    static constexpr uint32_t kEnteringRetryDelayMs = 10U;

    static std::size_t Index(EnumT s) noexcept
    {
        return static_cast<std::size_t>(s);
    }

    bool PostIntent(EnumT to, bool /*from_owner*/) noexcept
    {
        if (Index(to) >= N) { return false; }
        if (!IsLegal(current_, to)) {
            ++illegal_count_;
            last_illegal_from_ = current_;
            last_illegal_to_   = to;
            return false;
        }
        intent_target_  = to;
        intent_pending_ = true;
        return true;
    }

    Owner*  owner_{nullptr};

    // State table & registration tracking.
    std::array<Actions, N> table_{};
    std::array<bool,    N> registered_{};
    bool                   finalized_{false};

    // Current / previous / phase.
    EnumT      current_;
    EnumT      previous_;
    StatePhase phase_;
    uint32_t   entered_at_ms_;

    // Pending owner-side intent (single slot; processed at start of Update).
    bool   intent_pending_{false};
    EnumT  intent_target_{};

    // Pending external intent (single-slot atomic inbox: bit7 = pending).
    std::atomic<uint8_t> pending_intent_{0};

    // Legal-transition matrix.
    std::array<uint64_t, N> allowed_{};

    // Per-state watchdog and dwell counters.
    std::array<uint32_t, N> visits_{};
    std::array<uint32_t, N> last_dwell_ms_{};
    std::array<uint32_t, N> total_dwell_ms_{};
    std::array<uint32_t, N> max_dwell_ms_{};

    bool                   max_dwell_breach_latched_{false};
    uint32_t               max_dwell_breach_count_{0};
    EnumT                  max_dwell_last_breach_state_{};

    // Diagnostics.
    uint32_t illegal_count_{0};
    EnumT    last_illegal_from_{};
    EnumT    last_illegal_to_{};

    uintptr_t owner_token_{0};
    uint32_t  cross_task_request_count_{0};

    uint32_t  re_register_count_{0};

    LastTransition<EnumT> last_transition_{};
};

} // namespace hf_utils

#endif /* HF_UTILS_GENERAL_STATEMACHINE_H_ */
