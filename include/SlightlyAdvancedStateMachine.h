/**
 *
 * Nebula Tech Corporation
 *
 * Copyright © 2023 Nebula Tech Corporation. All Rights Reserved.
 * This file is part of HardFOC and is licensed under the GNU General Public License v3.0 or later.
 *
 */

#ifndef UTILITIES_COMMON_SLIGHTLYADVANCEDSTATEMACHINE_H_
#define UTILITIES_COMMON_SLIGHTLYADVANCEDSTATEMACHINE_H_

#include <unordered_map>
#include <functional>

#include "UTILITIES/common/SimpleStateMachine.h"
#include "UTILITIES/common/TimeStampedVariable.h"
#include "UTILITIES/common/EnumArray.h"
#include "UTILITIES/common/StateActionsBase.h"

/**
 * @class SlightlyAdvancedStateMachine
 * @brief This class represents a slightly advanced state machine.
 *
 * This class extends the SimpleStateMachine class and adds additional functionality for managing state actions.
 *
 * @tparam EnumType The type of the enum used for the states
 */
template <typename EnumType, size_t NumberOfStates>
class SlightlyAdvancedStateMachine : private SimpleStateMachine<EnumType> {
public:
    //==============================================================//
    /// STATE ACTIONS CONSTRUCTORS
    //==============================================================//

    /**
     * @brief Constructor with initial state
     *
     * Initializes the state machine with the provided initial state.
     *
     * @param initialState The initial state of the state machine
     */
    SlightlyAdvancedStateMachine(EnumType initialState) noexcept
        : SimpleStateMachine<EnumType>(initialState)
    { }

    /**
     * @brief Destructor
     *
     * Cleans up the state machine.
     */
    ~SlightlyAdvancedStateMachine() noexcept
    { }

    //==============================================================//
    /// ACCESSIBLE SIMPLE STATE MACHINE FUNCTIONS
    //==============================================================//

    using SimpleStateMachine<EnumType>::GetCurrAction;
    using SimpleStateMachine<EnumType>::IsCurrAction;
    using SimpleStateMachine<EnumType>::GetCurrentState;
    using SimpleStateMachine<EnumType>::GetPreviousState;
    using SimpleStateMachine<EnumType>::GetNextState;
    using SimpleStateMachine<EnumType>::GetRequestedState;
    using SimpleStateMachine<EnumType>::IsCurrentState;
    using SimpleStateMachine<EnumType>::IsPreviousState;
    using SimpleStateMachine<EnumType>::IsNextStateSet;
    using SimpleStateMachine<EnumType>::IsNextState;
    using SimpleStateMachine<EnumType>::IsRequestedState;
    using SimpleStateMachine<EnumType>::SetNextState;
    using SimpleStateMachine<EnumType>::SetRequestedState;
    using SimpleStateMachine<EnumType>::ClearRequestedState;
    using SimpleStateMachine<EnumType>::LockOwnership;
    using SimpleStateMachine<EnumType>::UnlockOwnership;

    //==============================================================//
    /// STATE ACTIONS MANAGERS
    //==============================================================//

    /**
     * @brief Register a state with its associated functions
     *
     * This method allows you to associate a set of functions (entry, loop, exit) with a specific state.
     *
     * @param State The state to register
     * @param stateFunctions The functions associated with the state
     */
    void RegisterStateActions(EnumType State, StateActionsBase& stateFunctions) noexcept;

    /**
     * @brief Unregister a state by removing it from the map
     *
     * This method allows you to remove the association between a state and its functions.
     *
     * @param State The state to unregister
     */
    void UnregisterStateActions(EnumType State) noexcept;

    //==============================================================//
    /// AUTO STATE MACHINE RUNNER [WITH REGISTERED ACTIONS]
    //==============================================================//

    /**
     * @brief If users would like to give up their manual controls of state transitions, update will handle all.
     *
     * This method automatically handles state transitions and calls the appropriate functions for each state.
     *
     * @return Time for thread loop to wait before looping back in Msec.
     */
    uint32_t Update(bool* steppedIntoNewState = nullptr, bool* stateActionsExistsArg = nullptr, bool* stateLoopFunctionsExistsArg = nullptr) noexcept;

    //==============================================================//
    /// STATE MACHINE RUNNING HELPERS
    //==============================================================//

    /**
     * @brief Step to the next state
     *
     * This method allows you to manually transition to the next state.
     *
     * @return True if successful, false otherwise
     */
    bool StepToNextState() override;

    /**
     * @brief Loop function
     *
     * This method continuously calls the loop function for the current state.
     *
     * @return The result of the loop function
     */
    uint32_t LoopCurrentState(bool* stateActionsExistsArg = nullptr, bool* stateLoopFunctionsExistsArg = nullptr) noexcept;

    //==============================================================//
    /// CURRENT STATE ACTIONS RUNNERS
    //==============================================================//

    /**
     * @brief Call the entry function of a state
     *
     * This method allows you to manually call the entry function for a specific state.
     *
     * @param state The state whose entry function is to be called
     * @return True if entry function exists and able to call it. False otherwise.
     */
    bool CallStateEntryFunction(EnumType state, bool& entryReturn) noexcept;

    /**
     * @brief Call the loop function of a state
     *
     * This method allows you to manually call the loop function for a specific state.
     *
     * @param state The state whose loop function is to be called
     * @return True if loop function exists and able to call it. False otherwise.
     */
    bool CallStateLoopFunction(EnumType state, uint32_t& loopReturn) noexcept;

    /**
     * @brief Call the exit function of a state
     *
     * This method allows you to manually call the exit function for a specific state.
     *
     * @param state The state whose exit function is to be called
     * @return True if exit function exists and able to call it. False otherwise.
     */
    bool CallStateExitFunction(EnumType state, bool& exitReturn) noexcept;

    //==============================================================//
    /// CURRENT STATE ACTIONS NULIFIERS
    //==============================================================//

    /**
     * @brief Nullify the entry function of a state
     *
     * This method allows you to remove the entry function for a specific state.
     *
     * @param state The state whose entry function is to be nullified
     */
    void NullifyEntryFunction(EnumType state) noexcept;

    /**
     * @brief Nullify the loop function of a state
     *
     * This method allows you to remove the loop function for a specific state.
     *
     * @param state The state whose loop function is to be nullified
     */
    void NullifyLoopFunction(EnumType state) noexcept;

    /**
     * @brief Nullify the exit function of a state
     *
     * This method allows you to remove the exit function for a specific state.
     *
     * @param state The state whose exit function is to be nullified
     */
    void NullifyExitFunction(EnumType state) noexcept;

    //==============================================================//
    /// RESETER
    //==============================================================//

    /**
     * @brief Just sets the current state to be in given state at entry actions.
     * @param state State to believe we just go in to.
     */
    void ResetStateInfoToState(EnumType state) noexcept;

    //==============================================================//
    /// GENERIC CHECKERS
    //==============================================================//

    /**
     * @brief Check if the state machine is in the specified action and state.
     *
     * @param action The action to check.
     * @param state The state to check.
     * @return True if the state machine is in the specified action and state, false otherwise.
     */
    bool IsStateInAction(state_machine_curr_action_t action, EnumType state) noexcept;

    /**
     * @brief Wait to see if the state machine is in the specified action and state.
     *
     * @param action The action to check.
     * @param state The state to check.
     * @param waitOptionMsec The maximum time to wait in milliseconds.
     * @param timeBetweenChecksMs The time between checks in milliseconds.
     * @return True if the state machine is in the specified action and state within the wait time, false otherwise.
     */
    bool WaitToSeeIfStateInAction(state_machine_curr_action_t action, EnumType state, uint32_t waitOptionMsec, uint32_t timeBetweenChecksMs) noexcept;

    //==============================================================//
    /// SPECIFIC CHECKERS
    //==============================================================//

    /**
     * @brief Check if the state machine is entering the specified state.
     *
     * @param stateArg The state to check.
     * @return True if the state machine is entering the specified state, false otherwise.
     */
    bool IsEnteringState(EnumType stateArg) noexcept;

    /**
     * @brief Check if the state machine is running the specified state.
     *
     * @param stateArg The state to check.
     * @return True if the state machine is running the specified state, false otherwise.
     */
    bool IsRunningState(EnumType stateArg) noexcept;

    /**
     * @brief Check if the state machine is leaving the specified state.
     *
     * @param stateArg The state to check.
     * @return True if the state machine is leaving the specified state, false otherwise.
     */
    bool IsLeavingState(EnumType stateArg) noexcept;


    /**
     * @brief Wait to see if the state machine is entering the specified state.
     *
     * @param stateArg The state to check.
     * @param waitOptionMsec The maximum time to wait in milliseconds.
     * @param timeBetweenChecksMs The time between checks in milliseconds.
     * @return True if the state machine is entering the specified state within the wait time, false otherwise.
     */
    bool WaitToSeeIfEnteringState(EnumType stateArg, uint32_t waitOptionMsec, uint32_t timeBetweenChecksMs) noexcept;

    /**
     * @brief Wait to see if the state machine is running the specified state.
     *
     * @param stateArg The state to check.
     * @param waitOptionMsec The maximum time to wait in milliseconds.
     * @param timeBetweenChecksMs The time between checks in milliseconds.
     * @return True if the state machine is running the specified state within the wait time, false otherwise.
     */
    bool WaitToSeeIfRunningState(EnumType stateArg, uint32_t waitOptionMsec, uint32_t timeBetweenChecksMs) noexcept;

    /**
     * @brief Wait to see if the state machine is leaving the specified state.
     *
     * @param stateArg The state to check.
     * @param waitOptionMsec The maximum time to wait in milliseconds.
     * @param timeBetweenChecksMs The time between checks in milliseconds.
     * @return True if the state machine is leaving the specified state within the wait time, false otherwise.
     */
    bool WaitToSeeIfLeavingState(EnumType stateArg, uint32_t waitOptionMsec, uint32_t timeBetweenChecksMs) noexcept;

    //==============================================================//
    //==============================================================//

private:

    EnumArray<EnumType, StateActionsBase* , NumberOfStates> statesActions;	///< Map of states to their associated functions

	static constexpr uint32_t DEFAULT_LOOP_FUNCTION_INTERVAL_MSEC = 500;				/**< Time to return when we enter loop and encounter an error by default. */
	static constexpr uint32_t DEFAULT_LOOP_FUNCTION_STATE_EXITING_INTERVAL_MSEC = 100;	/**< Time to return when we enter loop while the current state action is STATE_EXITING. */
};

//==============================================================//
/// STATE ACTIONS MANAGERS
//==============================================================//

template <typename EnumType, size_t NumberOfStates>
void SlightlyAdvancedStateMachine<EnumType,NumberOfStates>::RegisterStateActions(EnumType State, StateActionsBase& stateFunctions) noexcept {
	statesActions = {State, &stateFunctions};
}

/**
 * @brief Unregister a state by removing it from the map
 * @param State The state to unregister
 */
template <typename EnumType, size_t NumberOfStates>
void SlightlyAdvancedStateMachine<EnumType,NumberOfStates>::UnregisterStateActions(EnumType State) noexcept {
    statesActions[State] = nullptr;
}

//==============================================================//
/// AUTO STATE MACHINE RUNNER [WITH REGISTERED ACTIONS]
//==============================================================//

/**
 * @brief If users would like to give up their manual controls of state transitions, update will handle all.
 * @return Time for thread loop to wait before looping back in Msec.
 */
template <typename EnumType, size_t NumberOfStates>
uint32_t SlightlyAdvancedStateMachine<EnumType,NumberOfStates>::Update(bool* steppedIntoNewState, bool* stateActionsExistsArg, bool* stateLoopFunctionsExistsArg) noexcept {
	/// Go into
	bool status = StepToNextState();
	if(steppedIntoNewState) {
		*steppedIntoNewState = status;
	}
	return LoopCurrentState(stateActionsExistsArg, stateLoopFunctionsExistsArg);
}

//==============================================================//
/// STATE MACHINE RUNNING HELPERS
//==============================================================//

/**
 * @brief Step to the next state
 * @return True if successful, false otherwise
 */
template <typename EnumType, size_t NumberOfStates>
bool SlightlyAdvancedStateMachine<EnumType,NumberOfStates>::StepToNextState()
{
    bool success = false;
    bool stateActionsExists;

    /// If next state is set
    if(SimpleStateMachine<EnumType>::isNextStateSet)
    {
        /// If current thread is allowed to step state machine
        TX_THREAD *currentThread = tx_thread_identify();
        if (SimpleStateMachine<EnumType>::ownerThread == nullptr || SimpleStateMachine<EnumType>::ownerThread == currentThread)
        {
            MutexGuard guard(SimpleStateMachine<EnumType>::stateMutex);
			bool stateExitSuccess = false;

    		/// Let's set the current action to state exiting
    		SimpleStateMachine<EnumType>::SetCurrAction(state_machine_curr_action_t::STATE_EXITING);

    		/// Check if the currentState key exists in the statesActions map
    		stateActionsExists = (statesActions[SimpleStateMachine<EnumType>::currentState] != nullptr);

    		/// If it does,
    		if(stateActionsExists)
    		{
    			/// Call the exit function of the current state if it exists, and if not, still set flag to true
    			if ( (!statesActions[SimpleStateMachine<EnumType>::currentState]->ExitFunction)
    					|| (statesActions[SimpleStateMachine<EnumType>::currentState]->ExitFunction()) )
    			{
    				/// If state exit is successful, set variable
    				stateExitSuccess = true;
    			}
    		}
    		/// If state actions don't exist,
    		else {
    			/// state exit is taken as being successful.
				stateExitSuccess = true;
    		}

        	/// if the state exit is successful
        	if(stateExitSuccess)
        	{
        		/// Step into the next state
        		SimpleStateMachine<EnumType>::SetState(SimpleStateMachine<EnumType>::nextState);

                /// Reset flag
        		SimpleStateMachine<EnumType>::isNextStateSet = false;

        		/// And Let's set the current action to state entering
        		SimpleStateMachine<EnumType>::SetCurrAction(STATE_ENTERING);
    			bool stateEnteringSuccess = false;

        		/// Check if the new current State key exists in the statesActions map
        		stateActionsExists = (statesActions[SimpleStateMachine<EnumType>::currentState] != nullptr);

        		/// If it does,
        		if(stateActionsExists)
        		{
        			/// Call the entry function of the current state if it exists, and if not, still set flag to true
        			if ( (!statesActions[SimpleStateMachine<EnumType>::currentState]->EntryFunction)
        					|| (statesActions[SimpleStateMachine<EnumType>::currentState]->EntryFunction()) )
        			{
        				/// If state entry is successful, set variable
        				stateEnteringSuccess = true;
        			}
        		}
        		/// If state actions don't exist,
        		else {
        			/// state exit is taken as being successful.
        			stateEnteringSuccess = true;
        		}

        		/// If state entry succeeded, make sure to set the current action
        		if(stateEnteringSuccess) {
            		/// And Let's set the current action to state running
        			SimpleStateMachine<EnumType>::SetCurrAction(STATE_RUNNING);
        		}
        	}

    		/// Although the state entering status could be returned, we are technically in new state after exiting and
    		/// setting new state with the current actions as STATE_ENTERING. As far as StepToNextState is worried about
    		/// this has been successful, the LoopFunction will need to check if the current state has properly finished it's entry function.
        	success = stateExitSuccess;
        }
    }

    return success;
}

/**
 * @brief Loop function
 * @return The result of the loop function
 */
template <typename EnumType, size_t NumberOfStates>
uint32_t SlightlyAdvancedStateMachine<EnumType,NumberOfStates>::LoopCurrentState(bool* stateActionsExistsArg, bool* stateLoopFunctionsExistsArg) noexcept {
	/// Check if the currentState key exists in the statesActions map
	bool stateActionsExists = (statesActions[SimpleStateMachine<EnumType>::currentState] != nullptr);

	if(stateActionsExistsArg) {
		*stateActionsExistsArg = stateActionsExists;
	}

	/// If we're still currently in the state entering stage
	if(SimpleStateMachine<EnumType>::IsCurrAction(STATE_ENTERING))
	{
		bool stateEnteringSuccess = false;

		/// If state actions do exist,
		if(stateActionsExists)
		{

			/// Call the entry function of the current state if it exists, and if not, still set flag to true
			if ( (!statesActions[SimpleStateMachine<EnumType>::currentState]->EntryFunction)
					|| (statesActions[SimpleStateMachine<EnumType>::currentState]->EntryFunction()) )
			{
				/// If state entry is successful, set variable
				stateEnteringSuccess = true;
			}
		}
		/// If state actions don't exist,
		else {
			/// state exit is taken as being successful.
			stateEnteringSuccess = true;
		}

		/// If state entry succeeded, make sure to set the current action
		if(stateEnteringSuccess) {
			/// And Let's set the current action to state running
			SimpleStateMachine<EnumType>::SetCurrAction(STATE_RUNNING);
		}

		/// Loop back in a little bit to run the running algorithm.
		return 10U;
	}
	/// If we're in the state running stage
	else if(SimpleStateMachine<EnumType>::IsCurrAction(STATE_RUNNING) && stateActionsExists)
	{
		bool stateLoopFunctionExists = (statesActions[SimpleStateMachine<EnumType>::currentState]->LoopFunction);

		if(stateLoopFunctionsExistsArg) {
			(*stateLoopFunctionsExistsArg) = stateLoopFunctionExists;
		}

		/// And the loop function exists
		if(stateLoopFunctionExists)
		{
			/// Run it and return the delay time.
			return (statesActions[SimpleStateMachine<EnumType>::currentState]->LoopFunction());
		}
	}
	/// However if the state running is the exit stage
	else if(SimpleStateMachine<EnumType>::IsCurrAction(STATE_EXITING) && stateActionsExists)
	{
		/// The loop should currently do nothing and just return default time,
		/// but if we find it beneficial to have something here like exit and
		/// entry failure functions we can have them called here for exit failures.
		/// since the only time we'll be here in - In loop with current action STATE_EXITING
		/// is if the exit failed, so do nothing.
		return DEFAULT_LOOP_FUNCTION_STATE_EXITING_INTERVAL_MSEC;
	}

	/// If loop function does not exists, loop with a 500ms interval;
	return DEFAULT_LOOP_FUNCTION_INTERVAL_MSEC;
}

//==============================================================//
/// CURRENT STATE ACTIONS RUNNERS
//==============================================================//

/**
 * @brief Call the loop function of a state
 * @param state The state whose loop function is to be called
 * @return The result of the loop function
 */
template <typename EnumType, size_t NumberOfStates>
bool SlightlyAdvancedStateMachine<EnumType,NumberOfStates>::CallStateEntryFunction(EnumType state, bool& entryReturn) noexcept {
	/// Check if the state has registered actions
	bool stateActionsExists = (statesActions[state] != nullptr);
	if(stateActionsExists) {
		/// Check if the entry function exits
		bool StateEntryFunctionExits = (statesActions[state]->EntryFunction);
		if(StateEntryFunctionExits) {
			/// If it does, call it and return data/true
			entryReturn = (statesActions[state]->EntryFunction());
			return true;
		}
	}

	/// If unable to run an actual entry function return false
	return false;
}

/**
 * @brief Call the loop function of a state
 * @param state The state whose loop function is to be called
 * @return The result of the loop function
 */
template <typename EnumType, size_t NumberOfStates>
bool SlightlyAdvancedStateMachine<EnumType,NumberOfStates>::CallStateLoopFunction(EnumType state, uint32_t& loopReturn) noexcept {
	/// Check if the state has registered actions
	bool stateActionsExists = (statesActions[state] != nullptr);
	if(stateActionsExists) {
		/// Check if the loop function exits
		bool StateLoopFunctionExits = (statesActions[state]->LoopFunction);
		if(StateLoopFunctionExits) {
			/// If it does, call it and return data/true
			loopReturn = (statesActions[state]->LoopFunction());
			return true;
		}
	}

	/// If unable to run an actual loop function return false
	return false;
}

/**
 * @brief Call the loop function of a state
 * @param state The state whose loop function is to be called
 * @return The result of the loop function
 */
template <typename EnumType, size_t NumberOfStates>
bool SlightlyAdvancedStateMachine<EnumType,NumberOfStates>::CallStateExitFunction(EnumType state, bool& exitReturn) noexcept {
	/// Check if the state has registered actions
	bool stateActionsExists = (statesActions[state] != nullptr);
	if(stateActionsExists) {
		/// Check if the entry function exits
		bool StateExitFunctionExits = (statesActions[state]->ExitFunction);
		if(StateExitFunctionExits) {
			/// If it does, call it and return data/true
			exitReturn = (statesActions[state]->ExitFunction());
			return true;
		}
	}

	/// If unable to run an actual exit function return false
	return false;
}

//==============================================================//
/// CURRENT STATE ACTIONS NULIFIERS
//==============================================================//

template <typename EnumType, size_t NumberOfStates>
void SlightlyAdvancedStateMachine<EnumType,NumberOfStates>::NullifyEntryFunction(EnumType state) noexcept {
    /// Set the exit function to a no-op function
	statesActions[state].EntryFunctionPtr = nullptr;
}

template <typename EnumType, size_t NumberOfStates>
void SlightlyAdvancedStateMachine<EnumType,NumberOfStates>::NullifyLoopFunction(EnumType state) noexcept {
    /// Set the loop function to a no-op function
	statesActions[state].LoopFunctionPtr = nullptr;
}

template <typename EnumType, size_t NumberOfStates>
void SlightlyAdvancedStateMachine<EnumType,NumberOfStates>::NullifyExitFunction(EnumType state) noexcept {
    /// Set the exit function to a no-op function
	statesActions[state].ExitFunctionPtr = nullptr;
}

//==============================================================//
/// RESETER
//==============================================================//

template <typename EnumType, size_t NumberOfStates>
void SlightlyAdvancedStateMachine<EnumType,NumberOfStates>::ResetStateInfoToState(EnumType state) noexcept {
	/// Set the current action to entering.
	SimpleStateMachine<EnumType>::SetCurrAction(state_machine_curr_action_t::STATE_ENTERING);
	SimpleStateMachine<EnumType>::SetState(state);
}

//==============================================================//
/// GENERIC CHECKERS
//==============================================================//

template <typename EnumType, size_t NumberOfStates>
bool SlightlyAdvancedStateMachine<EnumType, NumberOfStates>::IsStateInAction(state_machine_curr_action_t action, EnumType state) noexcept {
    auto currStateAction = GetCurrAction();
    auto systemCurrState = GetCurrentState();
    return currStateAction == action && systemCurrState == state;
}

template <typename EnumType, size_t NumberOfStates>
bool SlightlyAdvancedStateMachine<EnumType, NumberOfStates>::WaitToSeeIfStateInAction(state_machine_curr_action_t action, EnumType state, uint32_t waitOptionMsec, uint32_t timeBetweenChecksMs) noexcept {
    std::function<bool()> CheckIfStateInAction = [this, action, state] {
        auto currStateAction = GetCurrAction();
        auto systemCurrState = GetCurrentState();
        return currStateAction == action && systemCurrState == state;
    };
    return TestLogicWithTimeout(CheckIfStateInAction, true, waitOptionMsec, timeBetweenChecksMs);
}

//==============================================================//
/// SPECIFIED CHECKERS
//==============================================================//

template <typename EnumType, size_t NumberOfStates>
bool SlightlyAdvancedStateMachine<EnumType, NumberOfStates>::IsEnteringState(EnumType stateArg) noexcept {
    return IsStateInAction(STATE_ENTERING, stateArg);
}

template <typename EnumType, size_t NumberOfStates>
bool SlightlyAdvancedStateMachine<EnumType, NumberOfStates>::IsRunningState(EnumType stateArg) noexcept {
    return IsStateInAction(STATE_RUNNING, stateArg);
}

template <typename EnumType, size_t NumberOfStates>
bool SlightlyAdvancedStateMachine<EnumType, NumberOfStates>::IsLeavingState(EnumType stateArg) noexcept {
    return IsStateInAction(STATE_EXITING, stateArg);
}

template <typename EnumType, size_t NumberOfStates>
bool SlightlyAdvancedStateMachine<EnumType, NumberOfStates>::WaitToSeeIfEnteringState(EnumType stateArg, uint32_t waitOptionMsec, uint32_t timeBetweenChecksMs) noexcept {
    return WaitToSeeIfStateInAction(STATE_ENTERING, stateArg, waitOptionMsec, timeBetweenChecksMs);
}

template <typename EnumType, size_t NumberOfStates>
bool SlightlyAdvancedStateMachine<EnumType, NumberOfStates>::WaitToSeeIfRunningState(EnumType stateArg, uint32_t waitOptionMsec, uint32_t timeBetweenChecksMs) noexcept {
    return WaitToSeeIfStateInAction(STATE_RUNNING, stateArg, waitOptionMsec, timeBetweenChecksMs);
}

template <typename EnumType, size_t NumberOfStates>
bool SlightlyAdvancedStateMachine<EnumType, NumberOfStates>::WaitToSeeIfLeavingState(EnumType stateArg, uint32_t waitOptionMsec, uint32_t timeBetweenChecksMs) noexcept {
    return WaitToSeeIfStateInAction(STATE_EXITING, stateArg, waitOptionMsec, timeBetweenChecksMs);
}


//==============================================================//
//==============================================================//


#endif /* UTILITIES_COMMON_SLIGHTLYADVANCEDSTATEMACHINE_H_ */
