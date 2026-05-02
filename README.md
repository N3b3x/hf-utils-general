# hf-utils-general

Hardware-agnostic, header-only C++ utility library for the HardFOC stack.
Pure C++17 standard library — no RTOS, no console I/O, no platform headers.

This library is the **deepest** node in the HardFOC submodule chain. It must
not include from any sibling or parent. See the layering rules in the project
[`AGENTS.md`](../../../../../../AGENTS.md).

## Building

There is no dedicated build system. Add `include/` to your include path and
compile any `.cpp` you use from `src/`. Example:

```bash
g++ -std=c++17 -Iinclude -c src/Utility.cpp
```

## Header summary

Required by project `AGENTS.md`: every header carries `@file` / `@brief`,
a thread-safety note, and an allocation-behavior note. The table below
collects the one-line `@brief` of each header — see the file itself for the
full Doxygen block.

### Math, control, and curve fitting

| Header | Purpose |
|---|---|
| [`PID.h`](include/PID.h) | Lightweight, header-only discrete PID controller |
| [`AveragingFilter.h`](include/AveragingFilter.h) | Templated moving-average filter |
| [`BoundedLinearCurve.h`](include/BoundedLinearCurve.h) | Linear equation restricted to a specific x-range |
| [`PiecewiseLinearCurve.h`](include/PiecewiseLinearCurve.h) | Piecewise linear curve composed of `BoundedLinearCurve` segments |
| [`PiecewiseBounds.h`](include/PiecewiseBounds.h) | Piecewise min / max bounds across multiple `BoundedLinearCurve` segments |
| [`LineEstimator.h`](include/LineEstimator.h) | Estimates the slope of a line from a stream of data points |
| [`ParabolicCurveEstimator.h`](include/ParabolicCurveEstimator.h) | Parabolic-curve fit via least squares regression |
| [`CrcCalculator.h`](include/CrcCalculator.h) | CRC-16 / CCITT-False over an input buffer |

### Containers and value wrappers

| Header | Purpose |
|---|---|
| [`DynamicArray.h`](include/DynamicArray.h) | Lightweight container with a fixed maximum capacity |
| [`CircularBuffer.h`](include/CircularBuffer.h) | Fixed-size circular buffer template |
| [`RingBuffer.h`](include/RingBuffer.h) | Alternative ring-buffer implementation (legacy; prefer `CircularBuffer`) |
| [`EnumArray.h`](include/EnumArray.h) | Generic array indexed by an enumeration type |
| [`MultibitSet.h`](include/MultibitSet.h) | Wrapper around `std::bitset` for multi-bit entries |
| [`EnumeratedSetStatus.h`](include/EnumeratedSetStatus.h) | Tagged Type / Status enumeration pair |
| [`TimestampedVariable.h`](include/TimestampedVariable.h) | Value of type `T` paired with a timestamp |
| [`VariableWithUnit.h`](include/VariableWithUnit.h) | Value of type `T` paired with a unit of type `U` |

### Variable monitoring

| Header | Purpose |
|---|---|
| [`VariableTrackerBase.h`](include/VariableTrackerBase.h) | Slope-calculation strategy base |
| [`VariableMonitor.h`](include/VariableMonitor.h) | Monitors values for slope / threshold anomalies |
| [`VariableAnomalyMonitor.h`](include/VariableAnomalyMonitor.h) | Adds anomaly classification on top of `VariableMonitor` |
| [`MultiReadings.h`](include/MultiReadings.h) | Manages a fixed-size set of sensor readings |

### State machines

| Header | Purpose |
|---|---|
| [`StateMachine.h`](include/StateMachine.h) | Allocation-free, type-safe finite state machine with Entry / Loop / Exit |
| [`SimpleStateMachine.h`](include/SimpleStateMachine.h) | Minimal state-machine helper |
| [`SlightlyAdvancedStateMachine.h`](include/SlightlyAdvancedStateMachine.h) | Slightly richer state-machine variant |
| [`StateActionsBase.h`](include/StateActionsBase.h) | Base type for per-state actions |

### Timing, scheduling, and lifecycle

| Header | Purpose |
|---|---|
| [`ActionTimer.h`](include/ActionTimer.h) | Measures elapsed time of an action |
| [`ActionRunLimiter.h`](include/ActionRunLimiter.h) | Limits how many times an action may execute |
| [`DestructAction.h`](include/DestructAction.h) | Runs a function on object destruction (RAII helper) |
| [`TaskManager.h`](include/TaskManager.h) | Task structure with priority, predicate, and executor |
| [`TestManager.h`](include/TestManager.h) | Manages a sequence of tests |
| [`SoftwareVersion.h`](include/SoftwareVersion.h) | Compile-time software version constants |
| [`Utility.h`](include/Utility.h) | Generic helper functions (incl. millisecond timer) |
| [`platform_compat.h`](include/platform_compat.h) | Small set of platform-portable type defs |

## `StateMachine` worked example

The canonical mid-tier finite state machine. Allocation-free, type-safe,
member-function-pointer hooks, optional legal-transition matrix, always-on
visit / dwell instrumentation, and a per-state stuck-state watchdog. Use this
for any new firmware-side state machine; reach for `SimpleStateMachine` /
`SlightlyAdvancedStateMachine` only when integrating with legacy code or when
you specifically need runtime-bound `std::function` hooks (heap-prone).

```cpp
#include "StateMachine.h"

class MyMode {
public:
    enum class S : uint8_t { Idle = 0, Arming, Active, COUNT };

    MyMode() noexcept : sm_(*this, S::Idle) {
        // Per-state Entry/Loop/Exit. nullptr = no-op success.
        sm_.Register(S::Idle,   {nullptr,
                                 &MyMode::LoopIdle,
                                 nullptr});
        sm_.Register(S::Arming, {&MyMode::EntryArming,
                                 &MyMode::LoopArming,
                                 &MyMode::ExitArming});
        sm_.Register(S::Active, {&MyMode::EntryActive,
                                 &MyMode::LoopActive,
                                 &MyMode::ExitActive});

        // Optional legal-transition matrix (bit `i` of mask[from] = legal `to`).
        std::array<uint64_t, 3> allowed{};
        allowed[Bit(S::Idle)]   = Bit(S::Arming);
        allowed[Bit(S::Arming)] = Bit(S::Active) | Bit(S::Idle);
        allowed[Bit(S::Active)] = Bit(S::Idle);
        sm_.SetTransitionMatrix(allowed);

        // Optional stuck-state watchdog (Active for >5 s = abnormal).
        sm_.SetMaxDwell(S::Active, 5'000U);

        // Bind owner task once we know it (in Setup()):
        // sm_.SetOwnerToken(reinterpret_cast<uintptr_t>(os_get_current_task()));

        // Sanity check; propagate failure to your Initialize() return value.
        (void)sm_.Finalize();
    }

    // Owner-task drives one tick (e.g. inside BaseThread::Step()).
    uint32_t Tick(uint32_t now_ms) noexcept { return sm_.Update(now_ms); }

    // Other tasks post intents through the lock-free single-slot inbox.
    void RequestArm()  noexcept { sm_.OnExternalIntent(S::Arming); }
    void RequestStop() noexcept { sm_.OnExternalIntent(S::Idle); }

private:
    static constexpr uint64_t Bit(S s) noexcept {
        return uint64_t{1} << static_cast<uint64_t>(s);
    }

    bool     EntryArming()  noexcept { /* arm hardware */ return true; }
    uint32_t LoopArming()   noexcept { /* poll arming   */ return 50U; }
    bool     ExitArming()   noexcept { /* tidy up       */ return true; }

    bool     EntryActive()  noexcept { /* enable PID    */ return true; }
    uint32_t LoopActive()   noexcept { /* control tick  */ return 10U; }
    bool     ExitActive()   noexcept { /* disable PID   */ return true; }

    uint32_t LoopIdle()     noexcept { return 100U; }

    hf_utils::StateMachine<MyMode, S, static_cast<size_t>(S::COUNT)> sm_;
};
```

### Hook contract

| Hook | Signature | Semantics |
|---|---|---|
| Entry | `bool Owner::*() noexcept` | Return `true` to advance into Running. `false` → keeps Entering; next tick re-attempts. |
| Loop  | `uint32_t Owner::*() noexcept` | Return value = advisory delay (ms) until next tick. Owner thread chooses whether to honour it. |
| Exit  | `bool Owner::*() noexcept` | Return `false` → aborts the transition (machine stays put), recorded in `LastTransition`. |

Any hook may be `nullptr` — treated as no-op success / `kDefaultLoopDelayMs`.

### Threading model

- **Single owner**: `RequestTransition()` and `Update()` must run on the owner
  task. Set `SetOwnerToken(taskHandleOrId)` to trip a counter on accidental
  cross-task `RequestTransition(...,caller_token)` calls.
- **External intents**: any other task uses `OnExternalIntent(target)`,
  which posts to a single-slot atomic inbox (last-writer-wins) drained on
  the next `Update()` from the owner.
- **No heap**: every storage element is `std::array`, sized at instantiation.

### What `Update()` does each tick

1. Drain pending external intent (if any) → reject if illegal (counter +
   `LastIllegalFrom/To`), otherwise convert to internal pending.
2. If pending: run `Exit(from)` (if any). On `false`, abort transition.
3. On exit OK: update dwell counters, swap state, run `Entry(to)`. On
   `Entry → false`, leave phase = `Entering`; next tick re-attempts.
4. While `Running`, check the optional max-dwell watchdog and bump the
   breach counter once per dwell window.
5. Run `Loop(current)` and return its requested loop delay.

### Diagnostics surface

Always-on, queryable from any task (read-only):

- `GetCurrentState()`, `GetCurrentPhase()`, `DwellMs(now_ms)`.
- `VisitCount(s)`, `LastDwellMs(s)`, `TotalDwellMs(s)`.
- `IllegalTransitionCount()`, `LastIllegalFrom()`, `LastIllegalTo()`.
- `MaxDwellBreachCount()`, `MaxDwellLastBreachState()`.
- `CrossTaskRequestCount()`, `ReRegisterCount()`.
- `GetLastTransition()` → `{from, to, at_ms, exit_ok, entry_ok}`.

A console `dump` command can pack any subset of these into a JSON / table
payload without touching internals — they are all const accessors.

### Memory budget

Per state: `sizeof(StateActions<Owner>)` (~24 B, three member-fn-ptrs) +
five `uint32_t` counters + one `uint64_t` allowed-mask = ~52 B. For `N=8`
states the whole machine fits in well under 500 B. Hard cap of `N ≤ 64`
states (one bit per `to` in the legal matrix).

### When to use which variant

| Variant | Pick when… | Allocation |
|---|---|---|
| `StateMachine` | A concrete owner type hosts the hooks and you can name them at compile time. **Default for new code.** | Zero. |
| `SlightlyAdvancedStateMachine` | Hooks are bound at runtime from disparate sources / lambdas with captures. | Heap-prone (`std::function`). |
| `SimpleStateMachine` | A bare counter-driven advance loop with no per-state hooks at all. | None. |

## Notes on thread-safety and allocation

These utilities are **building blocks**, not concurrency primitives. Unless a
specific header documents otherwise:

- No header takes a lock or assumes thread-safety. Callers serialise access.
- Stateful objects allocate once at construction (typically inline storage
  via templates) and do **not** allocate during steady-state operation.
- Containers (`DynamicArray`, `CircularBuffer`, `EnumArray`, `MultibitSet`)
  are fixed-capacity and never grow.

For thread-aware primitives (mutexes, queues, snapshots, error history) see
the sibling library [`hf-utils-rtos-wrap`](../hf-utils-rtos-wrap/README.md).

## License

GNU GPL v3.0. See [`LICENSE`](LICENSE).
