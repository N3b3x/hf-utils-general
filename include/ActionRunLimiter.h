/**
 * @file ActionRunLimiter.h
 * @brief Utility to limit how many times an action may execute.
 *
 * Nebula Tech Corporation
 *
 * Copyright © 2023 Nebula Tech Corporation. All Rights Reserved.
 * This file is part of HardFOC and is licensed under the GNU General Public
 * License v3.0 or later.
 */

#ifndef UTILITIES_COMMON_ACTIONRUNLIMITER_H_
#define UTILITIES_COMMON_ACTIONRUNLIMITER_H_

#include <atomic>
#include <functional>

/**
 * @brief Class to limit the number of times an action can be executed.
 *
 * The ActionRunLimiter class provides mechanisms to control the execution
 * of an action based on a specified maximum run count and a condition.
 *
 * Example:
 * \code{.cpp}
 * {
 * 	   uint32_t maxNumberOfCounts = 5;
 * 	   SomeClass sc;
 * 	   auto actionLambda = [&sc]() {return sc.someMemberFunction(); }
 *     ActionRunLimiter action(true, maxNumberOfCounts, actionLambda);
 * }
 * \endcode
 *
 */
class ActionRunLimiter
{
public:

    /**
     * @brief Constructor to initialize the ActionRunLimiter.
     *
     * @param startRunCondition Initial condition to allow action execution.
     * @param maxRunCount Maximum number of times the action can be executed.
     * @param action Lambda function representing the action to be executed.
     */
    ActionRunLimiter(bool startRunCondition, uint32_t maxRunCount, std::function<bool()> action):
    	condition_(startRunCondition),
		maxRunCount_(maxRunCount),
		actionRunCount_(0),
		action_(action)
    {
    	/// No code in constructor
    }

    /**
     * @brief Destructor.
     */
    ~ActionRunLimiter() = default;

    /**
     * @brief Allows the action to be ran when RunIfNeeded() is called.
     */
    void EnableActionRun() {
    	condition_ = true;
    }

    /**
     * @brief Disables the action from running when RunIfNeeded() is called.
     */
    void DisableActionRun() {
    	condition_ = false;
    }

    /**
     * @brief Run the action if the action is enabled and we haven't reached max run count specified.
     *
     * @return True if action ran, false otherwise
     */
    bool RunIfNeeded() {
    	if(CanRunAction() && action_()) {
    		MarkActionAsRun();
    		return true;
    	}

    	return false;
    }

    /**
     * @brief Resets the action number of times ran counter.
     */
    void Reset() {
    	actionRunCount_ = 0;
    }

    /**
     * @brief Sets the maximum number of times the action can be run
     *
     * @param count The max number of times desired.
     * @param reset	If true, the current action ran number of times will be reset
     */
    void SetMaxRunCount(uint32_t count, bool reset = false) {
    	maxRunCount_ = count;

    	if(reset){
    		Reset();
    	}
    }

    /**
     * @brief Gets the maximum number of times action is allowed to run.
     *
     * @return the maximum number of times action is allowed to run.
     */
    uint32_t GetMaxRunCount() {
    	return maxRunCount_;
    }

private:

    /**
     * @brief Returns true if the action can run.
     *
     * @return True if the action can run, false otherwise.
     */
    bool CanRunAction() {
    	return condition_ && (actionRunCount_ < maxRunCount_);
    }

    /**
     * @brief Call Each time action succeeds the action to increment ran counter.
     */
    void MarkActionAsRun() {
    	++actionRunCount_;
    }

    std::atomic<bool> condition_;			///< Check to make sure the action is even allowed to run in the first place

    std::atomic<uint32_t> maxRunCount_;		///< The maximum amount of times the action is allowed to run
    std::atomic<uint32_t> actionRunCount_;	///< The current count of times the action has run

    std::function<bool()> action_;			///< Lambda function to run.
};

#endif /* UTILITIES_COMMON_ACTIONRUNLIMITER_H_ */
