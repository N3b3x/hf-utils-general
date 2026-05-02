/**
 * @file ActionTimer.h
 * @brief Utility for measuring elapsed time of an action.
 *
 * @todo Add @copyright line once project copyright wording is finalised.
 */

#ifndef HF_UTILS_GENERAL_ACTIONTIMER_H_
#define HF_UTILS_GENERAL_ACTIONTIMER_H_

#include <cstdint>
#include "Utility.h"

/**
 * @brief Class to measure the duration of an action.
 *
 * The ActionTimer class provides mechanisms to start, stop, and measure the duration
 * of an action using the GetElapsedTimeMsec() function.
 *
 * Example usage:
 * \code{.cpp}
 * {
 *     ActionTimer timer;
 *     timer.Start();
 *     // Perform some action
 *     timer.Stop();
 *     uint32_t duration = timer.GetDuration();
 * }
 * \endcode
 */
class ActionTimer {
public:
    /**
     * @brief Constructor to initialize the ActionTimer.
     */
    ActionTimer() : startTime_(0), endTime_(0), running_(false) {}

    /**
     * @brief Starts the timer.
     *
     * Records the current elapsed time as the start time.
     */
    void Start() {
        startTime_ = GetElapsedTimeMsec();
        running_ = true;
    }

    /**
     * @brief Stops the timer.
     *
     * Records the current elapsed time as the end time.
     */
    void Stop() {
        if (running_) {
            endTime_ = GetElapsedTimeMsec();
            running_ = false;
        }
    }

    /**
     * @brief Gets the duration of the action.
     *
     * Calculates the duration between the start and end times.
     *
     * @return The duration in milliseconds.
     */
    uint32_t GetDuration() const {
        return running_ ? (GetElapsedTimeMsec() - startTime_) : (endTime_ - startTime_);
    }

private:
    uint32_t startTime_; ///< The start time of the action.
    uint32_t endTime_;   ///< The end time of the action.
    bool running_;       ///< Indicates whether the timer is currently running.
};

#endif /* HF_UTILS_GENERAL_ACTIONTIMER_H_ */
