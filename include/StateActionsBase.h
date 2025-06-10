/**
 *
 * Nebula Tech Corporation
 *
 * Copyright © 2023 Nebula Tech Corporation. All Rights Reserved.
 * This file is part of HardFOC and is licensed under the GNU General Public License v3.0 or later.
 *
 */

#ifndef UTILITIES_COMMON_STATEACTIONSBASE_H_
#define UTILITIES_COMMON_STATEACTIONSBASE_H_

#include <functional>

/**
 * @class StateActionsBase
 * @brief This class represents the base actions for a state.
 *
 * Each state has three actions associated with it: Entry, Loop, and Exit.
 * These actions are represented as functions within this class.
 */
class StateActionsBase {
public:
    std::function<bool()> EntryFunction; 		///< Function to be called on entry
    std::function<uint32_t()> LoopFunction;  	///< Function to be called in loop
    std::function<bool()> ExitFunction;  		///< Function to be called on exit

    /**
     * @brief Default constructor
     *
     * Initializes the state actions with no functions.
     */
    StateActionsBase()
        : EntryFunction(nullptr),
          LoopFunction(nullptr),
          ExitFunction(nullptr)
    { }

    /**
     * @brief Constructor with arguments
     *
     * Initializes the state actions with the provided functions.
     *
     * @param EntryFunctionArg Function to be called on entry to the state
     * @param LoopFunctionArg Function to be called continuously while in the state
     * @param ExitFunctionArg Function to be called on exit from the state
     */
    StateActionsBase(std::function<bool()> EntryFunctionArg,
                     std::function<uint32_t()> LoopFunctionArg,
                     std::function<bool()> ExitFunctionArg)
        : EntryFunction(EntryFunctionArg),
          LoopFunction(LoopFunctionArg),
          ExitFunction(ExitFunctionArg)
    { }

    /**
     * @brief Default destructor
     */
    ~StateActionsBase() = default;

    /**
     * @brief Set the entry function for the state
     *
     * @param EntryFunctionArg The function to be called when the state is entered
     */
    void SetEntryFunction(std::function<bool()> EntryFunctionArg) { EntryFunction = EntryFunctionArg; } ///< Set the entry function

    /**
     * @brief Set the loop function for the state
     *
     * @param LoopFunctionArg The function to be called continuously while in the state
     */
    void SetLoopFunction(std::function<uint32_t()> LoopFunctionArg) { LoopFunction = LoopFunctionArg; } ///< Set the loop function

    /**
     * @brief Set the exit function for the state
     *
     * @param ExitFunctionArg The function to be called when the state is exited
     */
    void SetExitFunction(std::function<bool()> ExitFunctionArg) { ExitFunction = ExitFunctionArg; } ///< Set the exit function
};

#endif /* UTILITIES_COMMON_STATEACTIONSBASE_H_ */
