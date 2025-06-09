/**
 *
 * Nebula Tech Corporation
 *
 * Copyright © 2023 Nebula Tech Corporation. All Rights Reserved.
 * This file is part of HardFOC and is licensed under the GNU General Public License v3.0 or later.
 *
 */

/**
 * @file SimpleStateMachine.h
 * @brief Header file for the SimpleStateMachine class.
 *
 * This header file contains the definition of the SimpleStateMachine class,
 * which implements a state machine that can be owned by a specific ThreadX thread.
 * The class provides methods to set and get the state in a thread-safe manner,
 * lock and unlock thread ownership, etc.
 */

#ifndef SIMPLE_STATE_MACHINE_H
#define SIMPLE_STATE_MACHINE_H

#include "UTILITIES/common/RtosCompat.h"
#include <math.h>

#include "UTILITIES/common/MutexGuard.h"
#include "UTILITIES/common/CommonIDs.h"
#include "UTILITIES/common/TxUtility.h"
#include "UTILITIES/common/TimeStampedVariable.h"

/**
 * @brief A simple state machine class with thread ownership.
 *
 * This class implements a state machine that can be owned by a specific ThreadX thread.
 * Ownership ensures that only the owning thread can change the state.
 */
template <typename T>
class SimpleStateMachine {
public:
    /**
     * @brief Constructs the SimpleStateMachine with an initial state.
     *
     * The constructor initializes the state machine with a given initial state.
     * It also attempts to create a mutex for thread-safe operations. If successful,
     * the stateMutexCreated flag is set to true.
     *
     * @param initialState The initial state to set.
     */
    SimpleStateMachine(T initialState)
        : previousState(initialState),
          currentState(initialState),
          nextState(initialState),
          isNextStateSet(false),
		  requestedState(initialState),
		  isRequestedStateSet(false),
		  currAction(STATE_RUNNING),
          ownerThread(nullptr),
          stateMutex( "StateMutex", typeid(T).name()),
		  initialized(false)
    {
    	/// No code at this time.
    }

    /**
     * @brief Destructor for SimpleStateMachine.
     *
     * The destructor deletes the mutex if it was successfully created.
     */
    virtual ~SimpleStateMachine()
    {
        /// No code at this time
    }

	/**
	 * @brief Helper function to support lazy initialization.
	 *
	 * @return true if initialized, false otherwise.
	 */
	bool EnsureInitialized() noexcept
	{
	    if (!initialized)
	    {
	        initialized = Initialize();
	    }
	    return initialized;
	}

	/**
	 * @brief Sets the current action that's being performed. Independent of state changes, must be set by user.
	 * @param action Action wanting to set.
	 */
	void SetCurrAction(state_machine_curr_action_t action) noexcept
	{
		currAction = action;
	}

	/**
	 * @brief Gets the current action that's being performed (set by class user).
	 */
	state_machine_curr_action_t GetCurrAction() noexcept
	{
		return currAction;
	}

	/**
	 * @brief Checks if the current action that's being performed (set by class user) is the one specified.
	 * @param action Action wanting to check for.
	 */
	bool IsCurrAction(state_machine_curr_action_t action) noexcept
	{
		return (currAction == action);
	}


    /**
     * @brief Gets the current state of the state machine.
     *
     * This method retrieves the current state in a thread-safe manner,
     * if the mutex was successfully created. Otherwise, it directly returns the current state.
     *
     * @return The current state.
     */
    T GetCurrentState(uint32_t* p_timestamp=nullptr) {
        T state;

        if(EnsureInitialized()) {
            MutexGuard guard(stateMutex);

            state = currentState;
            if(p_timestamp!=nullptr) {*p_timestamp = currentState.GetTimestamp();}
        }
        else {
            state = currentState;
            if(p_timestamp!=nullptr) {*p_timestamp = currentState.GetTimestamp();}
        }
        return state;
    }


    /**
     * @brief Gets the previous state of the state machine.
     *
     * This method retrieves the previous state in a thread-safe manner,
     * if the mutex was successfully created. Otherwise, it directly returns the previous state.
     *
     * @return The previous state.
     */
    T GetPreviousState(uint32_t* p_timestamp=nullptr) {
        T state;

        if(EnsureInitialized()) {
            MutexGuard guard(stateMutex);

            state = previousState;
            if(p_timestamp!=nullptr) {*p_timestamp = previousState.GetTimestamp();}
        }
        else {
            state = previousState;
            if(p_timestamp!=nullptr) {*p_timestamp = previousState.GetTimestamp();}
        }
        return state;
    }


    /**
     * @brief Gets the next state, or the current state if the next state isn't set.
     * @return The next state if set, otherwise the current state.
     */
    T GetNextState(uint32_t* p_timestamp=nullptr) {
    	T state;

        if(EnsureInitialized()) {
            MutexGuard guard(stateMutex);

            if(isNextStateSet) {
            	state = nextState;
                if(p_timestamp!=nullptr) {*p_timestamp = nextState.GetTimestamp();}
            }
            else {
            	state = previousState;
                if(p_timestamp!=nullptr) {*p_timestamp = previousState.GetTimestamp();}
            }
        }
        else {
            if(isNextStateSet) {
            	state = nextState;
                if(p_timestamp!=nullptr) {*p_timestamp = nextState.GetTimestamp();}
            }
            else {
            	state = previousState;
                if(p_timestamp!=nullptr) {*p_timestamp = previousState.GetTimestamp();}
            }
        }
        return state;
    }


    /**
     * @brief Gets the requested state of the state machine.
     *
     * This method retrieves the requested state in a thread-safe manner,
     * if the mutex was successfully created. Otherwise, it directly returns the requested state.
     *
     * @return The requested state.
     */
    T GetRequestedState(uint32_t* p_timestamp=nullptr) {
        T state;

        if(EnsureInitialized()) {
            MutexGuard guard(stateMutex);

            state = requestedState;
            if(p_timestamp!=nullptr) {*p_timestamp = requestedState.GetTimestamp();}
        }
        else {
            state = requestedState;
            if(p_timestamp!=nullptr) {*p_timestamp = requestedState.GetTimestamp();}
        }
        return state;
    }


    /**
     * @brief Checks supplied state is the current state of the state machine
     *
     * @param state State to be checked against
     * @return True if current state is state to be checked against, false otherwise.
     */
    bool IsCurrentState(T state) {
        if(EnsureInitialized()) {
            return (currentState == state);
        }
        else{
            return (currentState == state);
        }
    }

    /**
     * @brief Checks supplied state is the previous state of the state machine
     *
     * @param state State to be checked against
     * @return True if previous state is state to be checked against, false otherwise.
     */
    bool IsPreviousState(T state) {
        if(EnsureInitialized()) {
            return (previousState == state);
        }
        else{
            return (previousState == state);
        }
    }

    /**
     * @brief Checks if the next state is set.
     *
     * @return True if the next state is set, false otherwise.
     */
    bool IsNextStateSet() {
        if(EnsureInitialized()) {
            return (isNextStateSet);
        }
        else{
            return (isNextStateSet);
        }
    }

    /**
     * @brief Checks if the given state is the next state.
     *
     * @param state The state to check.
     * @return True if the given state is the next state, false otherwise.
     */
    bool IsNextState(T state) {
        if(EnsureInitialized()) {
            return (isNextStateSet && state == nextState);
        }
        else{
            return (isNextStateSet && state == nextState);
        }
    }

    /**
     * @brief Checks supplied state is the requested state of the state machine
     *
     * @param state State to be checked against
     * @return True if requested state is state to be checked against, false otherwise.
     */
    bool IsRequestedState(T state) {
        if(EnsureInitialized()) {
            if(isRequestedStateSet)
                return (requestedState == state);
            else
                return false;
        }
        else{
            if(isRequestedStateSet)
                return (requestedState == state);
            else
                return false;
        }
    }

    /**
     * @brief Sets a new state for the state machine.
     *
     * This method sets a new state in a thread-safe manner,
     * if the mutex was successfully created. Only the owning thread (or if no owner exists)
     * is allowed to change the state.
     *
     * @param newState The new state to set.
     * @return True if the state was successfully set, false otherwise.
     */
    bool SetState(T newState) {
        bool success = false;
        if(EnsureInitialized()) {
            MutexGuard guard(stateMutex);
            TX_THREAD *currentThread = tx_thread_identify();

            if (ownerThread == nullptr || ownerThread == currentThread) {
                previousState = currentState;
                currentState = newState;
                isNextStateSet = false; // Ensure that any set next state is invalidated
                success = true;
            }
        }
        return success;
    }

    bool SetNextState(T state) {
        bool success = false;
        if(EnsureInitialized()) {
            MutexGuard guard(stateMutex);
            TX_THREAD *currentThread = tx_thread_identify();

            if (ownerThread == nullptr || ownerThread == currentThread) {
                nextState = state;
                isNextStateSet = true;
                success = true;
            }
        }
        return success;
    }

    /**
     * @brief Sets a requested state for the state machine.
     *
     * This method sets a requested state in a thread-safe manner,
     * if the mutex was successfully created. Only the owning thread (or if no owner exists)
     * is allowed to change the state.
     *
     * @param requestedState The requested state.
     * @return True if the state was successfully set, false otherwise.
     */
    bool SetRequestedState(T reqState) {
        bool success = false;
        if(EnsureInitialized()) {
            MutexGuard guard(stateMutex);
            TX_THREAD *currentThread = tx_thread_identify();

            if (ownerThread == nullptr || ownerThread == currentThread) {
                requestedState = reqState;
                isRequestedStateSet = true;
                success = true;
            }
        }
        return success;
    }

    /**
     * @brief Checks supplied state is the current state of the state machine
     *
     * @param state State to be checked against
     * @return True if requested state is cleared, false otherwise.
     */
    bool ClearRequestedState() {
        if(EnsureInitialized()) {
            MutexGuard guard(stateMutex);
            isRequestedStateSet = false;
        }
        else{
            isRequestedStateSet = false;
        }
        return true;
    }

    virtual bool StepToNextState()
    {
        bool success = false;
        if(isNextStateSet) {
            success = SetState(nextState);
            if(success) {
				MutexGuard guard(stateMutex);
				isNextStateSet = false; // Reset flag
            }
        }
        return success;
    }

    /**
     * @brief Locks the ownership to the current thread.
     *
     * If the mutex is created and there is no current owner, this function
     * sets the current ThreadX thread as the owner and returns true.
     *
     * @return True if the ownership was successfully locked, false otherwise.
     */
    bool LockOwnership() {
        if(EnsureInitialized()) {
            MutexGuard guard(stateMutex);

            if (ownerThread == nullptr) {
                ownerThread = tx_thread_identify();
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Unlocks the ownership.
     *
     * If the mutex is created and the current ThreadX thread is the owner,
     * this function releases the ownership and returns true.
     *
     * @return True if the ownership was successfully unlocked, false otherwise.
     */
    bool UnlockOwnership() {
        if(EnsureInitialized()) {
            MutexGuard guard(stateMutex);

            if (ownerThread == tx_thread_identify()) {
                ownerThread = nullptr;
                return true;
            }
        }
        return false;
    }

protected:

	/**
	 * @brief Function to initialize class.
	 * Generally, the initialize function should return false rather than causing a low-level fault.
	 *
	 * @return true if able to initialize, false otherwise.
	 */
	bool Initialize() noexcept
	{
		bool success = false;

		MutexGuard guard(stateMutex, MutexGuard::MaxInitializationTimeMsec, &success);  // Create the mutex and lock it, check for success

		return success;
	}

    TimestampedVariable<T> previousState; 	///< Previous state.
    TimestampedVariable<T> currentState;  	///< Current state.
    TimestampedVariable<T> nextState;   	///< Next state.
    bool isNextStateSet;        			///< Flag to indicate that the next state is set

    TimestampedVariable<T> requestedState;	///< Requested state.
    bool isRequestedStateSet; 				///< Flag to indicate that the next state is set

    state_machine_curr_action_t currAction; ///< Variable to store current action, must be manually stepped or set by user.

    TX_THREAD *ownerThread;     ///< Owning thread.
    Mutex stateMutex;           ///< Mutex for state and ownership.

    bool initialized;			///< To make sure class is initialized.
};

#endif // SIMPLE_STATE_MACHINE_H
