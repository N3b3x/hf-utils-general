/**
 * @file DestructAction.h
 * @brief Provides a utility for executing a function upon destruction of an object.
 * @author Nebiyu
 *
 * The DestructAction class is a utility designed to trigger a specific action when
 * an instance of it goes out of scope. This ensures certain actions are performed
 * when exiting a scope, irrespective of how the scope is exited, such as through
 * a regular flow, early return, exception handling, and more.
 *
 * Leveraging the power of C++ templates and lambda captures, the DestructAction
 * class is capable of managing a broad array of callable entities, including but
 * not limited to:
 * - Regular free functions
 * - Member functions (given an object or its pointer)
 * - Lambda functions
 * - Functors (objects that can be called as if they are ordinary functions)
 *
 * The following are illustrative examples demonstrating the utility of the
 * DestructAction class:
 *
 * @section member_func_example Example Usage with a Member Function:
 * @code
 * MyClass myClass;
 * {
 *     DestructAction action(&MyClass::printMessage, &myClass, "Called upon destruction!", 3);
 * } // action's destructor is invoked here, thereby calling the designated member function
 * @endcode
 *
 * @section free_func_example Example Usage with a Free Function:
 * @code
 * {
 *     DestructAction action(&freeFunction, "Hello from free function!");
 * } // action's destructor is triggered here, leading to the execution of the provided free function
 * @endcode
 *
 * @section lambda_example Example Usage with a Lambda:
 * @code
 * {
 *     DestructAction action([]() { std::cout << "Hello from lambda!"; });
 * } // action's destructor is called here, running the lambda function
 * @endcode
 *
 */


#ifndef DESTRUCTACTION_H
#define DESTRUCTACTION_H

#include <functional>
#include <tuple>

#if __cplusplus >= 201703L  // C++17 and above
#include <utility>

template <typename Func, typename... Args>
/**
 * @class DestructAction
 * @brief A utility class to invoke a specified function upon destruction.
 *
 * @tparam Func The type deduced for the function/callable to be executed.
 * @tparam Args The types deduced for the arguments of the function/callable.
 */
template <typename Func, typename... Args>
class DestructAction {
public:
    /**
     * @brief Constructs the DestructAction object and stores the function and its arguments.
     *
     * @param func The function or callable object to be executed upon destruction.
     * @param args Arguments for the function or callable object.
     */
    DestructAction(Func&& func, Args&&... args)
        : m_func(std::forward<Func>(func)), m_args(std::make_tuple(std::forward<Args>(args)...)) {}

    /**
     * @brief Destructs the DestructAction object and invokes the specified function.
     *
     * The function/callable is invoked with the specified arguments.
     * The function's return value (if any) is discarded.
     */
    ~DestructAction() {
        std::apply(
            [&](auto&&... args) {
                std::invoke(m_func, std::forward<decltype(args)>(args)...);
            },
            m_args
        );
    }

private:
    Func m_func; ///< The function or callable object to be executed upon destruction.
    std::tuple<Args...> m_args; ///< Tuple holding arguments for the function/callable.
};
#else  // C++11/14

/**
 * @class DestructAction
 * @brief A utility class to invoke a specified function upon destruction.
 *
 * @tparam Func The type deduced for the function/callable to be executed.
 * @tparam Args The types deduced for the arguments of the function/callable.
 */
template <typename... Args>
class DestructAction {
public:

    /**
     * @brief Constructs the DestructAction object using a generic callable.
     *
     * This constructor is suitable for any callable object (like lambdas) that
     * matches the signature `void()`. It provides flexibility in terms of accepting
     * lambdas, functors, or any other callable object without arguments and with a
     * void return type.
     *
     * @tparam Callable The type deduced for the callable to be executed. Typically
     *                  used for lambdas, functors, or any other callable objects.
     * @param callable The callable object to be executed upon destruction.
     */
    template <typename Callable>
    DestructAction(Callable&& callable) : m_action(std::forward<Callable>(callable)) {}

    /**
     * @brief Constructs the DestructAction object and stores the function and its arguments.
     *
     * @param func The function or callable object to be executed upon destruction.
     * @param args Arguments for the function or callable object.
     */
    template <typename Func>
    DestructAction(Func&& func, Args... args) {
        m_action = [=]() { (void)func(args...); };
    }

    /**
     * @brief Constructs the DestructAction object and stores the function and its arguments.
     *
     * @param func The function or callable object to be executed upon destruction.
     * @param obj  Pointer to object if calling a class member function
     * @param args Arguments for the function or callable object.
     */
    template <typename Func, typename Obj>
    DestructAction(Func&& func, Obj* obj, Args... args) {
        m_action = [=]() { (obj->*func)(args...); };
    }

    /**
     * @brief Constructs the DestructAction object for member functions without arguments.
     *
     * @param func The member function to be executed upon destruction.
     * @param obj  Pointer to object if calling a class member function.
     */
    template <typename Obj>
    DestructAction(void (Obj::*func)(), Obj* obj) {
        m_action = [=]() { (void)(obj->*func)(); };
    }

    /**
     * @brief Constructs the DestructAction object for member functions without arguments that have a return type.
     *
     * @param func The member function to be executed upon destruction.
     * @param obj  Pointer to object if calling a class member function.
     */
    template <typename Return, typename Obj>
    DestructAction(Return (Obj::*func)(), Obj* obj) {
        m_action = [=]() { (void)(obj->*func)(); };
    }

    /**
     * @brief Destructs the DestructAction object and invokes the stored action.
     *
     * The function/callable is invoked with the specified arguments.
     * The function's return value (if any) is discarded.
     */
    ~DestructAction() {
        m_action();
    }

private:
    std::function<void()> m_action; ///< The stored action to be executed upon destruction.
};

#endif  // C++ version check

#endif /* DESTRUCTACTION_H */
