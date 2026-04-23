/**
 * @file PID.h
 * @brief Lightweight, header-only discrete PID controller.
 *
 * Nebula Tech Corporation
 *
 * Copyright © 2026 Nebula Tech Corporation.   All Rights Reserved.
 * This file is part of HardFOC and is licensed under the GNU General Public
 * License v3.0 or later.
 *
 * Discrete PID controller intended for fixed-rate use inside a periodic
 * control task (typical pattern: one `Update()` call per `Step()` body of
 * a `BaseThread`-style worker). The implementation is deliberately tiny
 * (~80 LOC, single header, no dynamic allocation, no platform dependencies)
 * so it can be embedded in any HardFOC product without pulling extra deps.
 *
 * Features:
 *   - Configurable Kp / Ki / Kd, sample period, output clamps, integrator
 *     clamps.
 *   - **Derivative on measurement** (rather than on error) so a step-change
 *     in the setpoint does not produce a derivative kick.
 *   - **Anti-windup via back-calculation** when the output saturates: the
 *     amount of overshoot in the unsaturated control output is removed
 *     from the integrator, so the integrator never grows past what the
 *     output can act on.
 *   - **Optional first-order low-pass filter on the derivative term**
 *     (`deriv_filter_tau_s > 0` enables it) — useful when the measured
 *     signal is noisy.
 *   - `Reset()` zeros the integrator and the derivative history so the
 *     next `Update()` starts clean (useful when re-engaging a loop after
 *     a disable).
 *
 * Usage:
 * @code
 *   PID::Config cfg;
 *   cfg.kp    = 1.5f;
 *   cfg.ki    = 0.2f;
 *   cfg.kd    = 0.05f;
 *   cfg.dt_s  = 0.005f;        // 200 Hz loop
 *   cfg.out_min = 0.0f;
 *   cfg.out_max = 1.0f;
 *   PID loop(cfg);
 *   loop.SetSetpoint(target_psi);
 *   // ...inside Step():
 *   const float u = loop.Update(measured_psi);
 * @endcode
 */
#ifndef PID_h
#define PID_h

#include <algorithm>

class PID
{
public:
    struct Config
    {
        float kp = 0.0f;
        float ki = 0.0f;
        float kd = 0.0f;
        float dt_s = 0.001f;              ///< Sample period (seconds)
        float out_min = 0.0f;             ///< Output clamp lo
        float out_max = 1.0f;             ///< Output clamp hi
        float deriv_filter_tau_s = 0.0f;  ///< 0 disables derivative filtering
        float i_min = -1e30f;             ///< Integrator clamp lo
        float i_max = +1e30f;             ///< Integrator clamp hi
    };

    PID() noexcept = default;
    explicit PID(const Config& cfg) noexcept : cfg_(cfg) {}

    void Configure(const Config& cfg) noexcept { cfg_ = cfg; }

    /// Zero the integrator and derivative history. Call before re-engaging
    /// a previously disabled loop to get a bumpless start.
    void Reset() noexcept
    {
        integral_    = 0.0f;
        last_meas_   = 0.0f;
        last_d_filt_ = 0.0f;
        first_step_  = true;
    }

    void SetSetpoint(float sp) noexcept { setpoint_ = sp; }
    [[nodiscard]] float Setpoint() const noexcept { return setpoint_; }

    /// One control-loop iteration. Pass the latest measured value; returns
    /// the saturated control output.
    float Update(float measurement) noexcept
    {
        const float err = setpoint_ - measurement;

        integral_ += err * cfg_.dt_s;
        integral_ = std::clamp(integral_, cfg_.i_min, cfg_.i_max);

        // Derivative on measurement (not on error) — avoids a derivative
        // kick on setpoint changes. First step has no history → derivative
        // contribution is zero.
        float d_raw = 0.0f;
        if (!first_step_ && cfg_.dt_s > 0.0f)
        {
            d_raw = -(measurement - last_meas_) / cfg_.dt_s;
        }
        first_step_ = false;
        last_meas_  = measurement;

        // Optional first-order low-pass on derivative term.
        float d_filt = d_raw;
        if (cfg_.deriv_filter_tau_s > 0.0f)
        {
            const float a = cfg_.dt_s / (cfg_.deriv_filter_tau_s + cfg_.dt_s);
            d_filt = last_d_filt_ + a * (d_raw - last_d_filt_);
        }
        last_d_filt_ = d_filt;

        const float u_unsat = cfg_.kp * err + cfg_.ki * integral_ + cfg_.kd * d_filt;
        const float u       = std::clamp(u_unsat, cfg_.out_min, cfg_.out_max);

        // Anti-windup: back-calculate the integrator so it never grows past
        // what the saturated output can act on.
        if (cfg_.ki != 0.0f && u != u_unsat)
        {
            const float overshoot = (u_unsat - u) / cfg_.ki;
            integral_ -= overshoot;
        }

        return u;
    }

private:
    Config cfg_{};
    float  setpoint_{0.0f};
    float  integral_{0.0f};
    float  last_meas_{0.0f};
    float  last_d_filt_{0.0f};
    bool   first_step_{true};
};

#endif // PID_h
