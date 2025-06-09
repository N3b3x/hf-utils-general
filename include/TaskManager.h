/**
 *
 * Nebula Tech Corporation
 *
 * Copyright © 2023 Nebula Tech Corporation. All Rights Reserved.
 * This file is part of HardFOC and is licensed under the GNU General Public License v3.0 or later.
 *
 */

#ifndef UTILITIES_COMMON_TASKMANAGER_H_
#define UTILITIES_COMMON_TASKMANAGER_H_

#include <functional>
#include <array>
#include <algorithm>

/**
* @brief Task structure to hold priority, checking function, and execution function.
*/
struct Task {
   int priority;                        ///< Task priority (lower value means higher priority).
   std::function<bool()> needToDo;      ///< Function to check if the task needs to be done.
   std::function<void()> execute;       ///< Function to execute the task.

   /**
    * @brief Comparison operator for sorting tasks by priority.
    * @param other The other task to compare to.
    * @return True if this task has higher priority (lower value) than the other task.
    */
   bool operator<(const Task& other) const {
       return priority < other.priority;
   }
};

/**
* @brief TaskManager class to manage and execute tasks based on their priority.
*
* @tparam N The number of tasks.
*/
template<size_t N>
class TaskManager {
public:
   /**
    * @brief Constructor to initialize the TaskManager with a list of tasks.
    *
    * @param tasks Array of tasks to manage.
    */
   TaskManager(const std::array<Task, N>& tasksArg) noexcept;

   /**
    * @brief Execute the next highest priority task that needs to be done.
    *
    * @return True if a task was executed, false otherwise.
    */
   bool ExecuteNextTask() noexcept;

   /**
    * @brief Execute all tasks that need to be executed.
    *
    * @return True if a task was executed, false otherwise.
    */
   bool ExecuteAllNeededTasks() noexcept;

private:
   std::array<Task, N> tasks; ///< Array to hold tasks.
};

template<size_t N>
TaskManager<N>::TaskManager(const std::array<Task, N>& tasksArg) noexcept : tasks(tasksArg) {
   /// Sort tasks by priority
   std::sort(this->tasks.begin(), this->tasks.end());
}

template<size_t N>
bool TaskManager<N>::ExecuteNextTask() noexcept {
   for (const auto& task : tasks) {
       if (task.needToDo()) {
           task.execute();
           return true; /// Return true if a task was executed
       }
   }
   return false; /// Return false if no task was executed
}

template<size_t N>
bool TaskManager<N>::ExecuteAllNeededTasks() noexcept {
	bool taskExecuted = false;
   for (const auto& task : tasks) {
       if (task.needToDo()) {
           task.execute();
           taskExecuted = true; /// Set to true if a task was executed
       }
   }
   return taskExecuted; /// Return false if no task was executed
}

#endif /* UTILITIES_COMMON_TASKMANAGER_H_ */
