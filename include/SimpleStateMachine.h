#ifndef SIMPLE_STATE_MACHINE_H
#define SIMPLE_STATE_MACHINE_H

#include <math.h>
#include "TimestampedVariable.h"

/** Simple state machine without threading primitives. */
template <typename T>
class SimpleStateMachine {
public:
    explicit SimpleStateMachine(T initialState) noexcept
        : previousState(initialState),
          currentState(initialState),
          nextState(initialState),
          isNextStateSet(false),
          requestedState(initialState),
          isRequestedStateSet(false),
          currAction(STATE_RUNNING)
    {}

    virtual ~SimpleStateMachine() = default;

    void SetCurrAction(state_machine_curr_action_t action) noexcept { currAction = action; }
    state_machine_curr_action_t GetCurrAction() noexcept { return currAction; }
    bool IsCurrAction(state_machine_curr_action_t action) noexcept { return currAction == action; }

    T GetCurrentState(uint32_t* p_timestamp=nullptr) {
        if(p_timestamp) { *p_timestamp = currentState.GetTimestamp(); }
        return currentState;
    }

    T GetPreviousState(uint32_t* p_timestamp=nullptr) {
        if(p_timestamp) { *p_timestamp = previousState.GetTimestamp(); }
        return previousState;
    }

    T GetNextState(uint32_t* p_timestamp=nullptr) {
        if(isNextStateSet) {
            if(p_timestamp) { *p_timestamp = nextState.GetTimestamp(); }
            return nextState;
        }
        if(p_timestamp) { *p_timestamp = currentState.GetTimestamp(); }
        return currentState;
    }

    T GetRequestedState(uint32_t* p_timestamp=nullptr) {
        if(p_timestamp) { *p_timestamp = requestedState.GetTimestamp(); }
        return requestedState;
    }

    bool IsCurrentState(T state) { return currentState == state; }
    bool IsPreviousState(T state) { return previousState == state; }
    bool IsNextStateSet() { return isNextStateSet; }
    bool IsNextState(T state) { return isNextStateSet && state == nextState; }
    bool IsRequestedState(T state) { return isRequestedStateSet && requestedState == state; }

    bool SetState(T newState) {
        previousState = currentState;
        currentState = newState;
        isNextStateSet = false;
        return true;
    }

    bool SetNextState(T state) {
        nextState = state;
        isNextStateSet = true;
        return true;
    }

    bool SetRequestedState(T reqState) {
        requestedState = reqState;
        isRequestedStateSet = true;
        return true;
    }

    bool ClearRequestedState() {
        isRequestedStateSet = false;
        return true;
    }

    virtual bool StepToNextState() {
        if(isNextStateSet) {
            SetState(nextState);
            isNextStateSet = false;
            return true;
        }
        return false;
    }

protected:
    TimestampedVariable<T> previousState;
    TimestampedVariable<T> currentState;
    TimestampedVariable<T> nextState;
    bool isNextStateSet;

    TimestampedVariable<T> requestedState;
    bool isRequestedStateSet;

    state_machine_curr_action_t currAction;
};

#endif // SIMPLE_STATE_MACHINE_H
