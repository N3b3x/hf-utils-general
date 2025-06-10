/**
 *
 * Nebula Tech Corporation
 *
 * Copyright © 2025 Nebula Tech Corporation. All Rights Reserved.
 * This file is part of HardFOC and is licensed under the GNU General Public License v3.0 or later.
 *
 */

#ifndef TIMESTAMPED_VARIABLE_H /// Include guard to prevent double inclusion
#define TIMESTAMPED_VARIABLE_H

#include <cstdint> /// for uint32_t
#include "Utility.h"
#include <cmath>

/**
 * @class TimestampedVariable
 *
 * @tparam T The type of the value to be stored. Can be any type like int, float, etc.
 *
 * @brief A class template that stores a value of type T and an associated timestamp.
 *
 * This class provides a way to associate a timestamp with a variable. The timestamp
 * is automatically updated when the variable's value is modified.
 */
template <typename T>
class TimestampedVariable {
private:
    T value;              ///< The value stored in the variable.
    uint32_t timestamp;   ///< The timestamp associated with the value.

public:
    /**
     * @brief Default constructor.
     *
     * Initializes the value to its default and sets the timestamp to the current time.
     */
    TimestampedVariable() noexcept;

    /**
     * @brief Overloaded constructor.
     *
     * Initializes the value to the given parameter and sets the timestamp to the current time.
     *
     * @param initialValue The initial value for the variable.
     */
    TimestampedVariable(const T& initialValue) noexcept;

    TimestampedVariable(const TimestampedVariable& initialValue) noexcept = default;

    /**
     * @brief Setter function to update the value.
     *
     * Sets a new value for the variable and updates the timestamp.
     *
     * @param newValue The new value to set.
     */
    void SetValue(const T& newValue) noexcept;

    /**
     * @brief Getter function for the value.
     *
     * @return T The current value of the variable.
     */
    T GetValue() const noexcept;

    /**
     * @brief Getter function for a modifiable reference to the value.
     *
     * @return T& A modifiable reference to the stored value.
     */
    T& GetRef() noexcept;

    /**
     * @brief Getter function for a modifiable reference to the timestamp.
     *
     * @return uint32_t& A modifiable reference to the stored timestamp.
     */
    uint32_t& GetTimestampRef() noexcept;

    /**
     * @brief Getter function for the timestamp.
     *
     * @return uint32_t The current timestamp of the variable.
     */
    uint32_t GetTimestamp() const noexcept;

    /**
     * @brief Implicit conversion to the underlying value type T.
     *
     * Allows the object to be automatically converted to its stored value type.
     *
     * @return T The stored value.
     */
    operator T() const noexcept;

    /**
     * @brief Overload the assignment operator (=).
     *
     * This function allows you to easily set a new value for the variable
     * while simultaneously updating the timestamp.
     *
     * @param newValue The new value to be set.
     * @return TimestampedVariable& A reference to the current object.
     */
    TimestampedVariable& operator=(const T& newValue) noexcept;

    /**
     * @brief Overload the assignment operator (=).
     *
     * This function allows you to easily set a new value for the variable
     * while simultaneously updating the timestamp.
     *
     * @param other The other TimestampedVariable object to be copied.
     * @return TimestampedVariable& A reference to the current object.
     */
    TimestampedVariable& operator=(const TimestampedVariable& other) noexcept;

    /**
     * @brief Overload the equality operator (==).
     *
     * Compares the stored value with another value.
     *
     * @param rhs The value to compare against.
     * @return bool True if the stored value equals the given value, false otherwise.
     */
    bool operator==(const T& rhs) const noexcept;

    /**
     * @brief Overload the equality operator (==).
     *
     * Compares the stored value with another TimestampedVariable's value.
     *
     * @param other The other TimestampedVariable to compare against.
     * @return bool True if the stored value equals the other TimestampedVariable's value, false otherwise.
     */
    bool operator==(const TimestampedVariable& other) const noexcept;

    /**
     * @brief Overload [] operator to access elements in array style
     *
     * @param rhs The value to compare against.
     * @return bool True if the stored value equals the given value, false otherwise.
     */
    T& operator[](int) noexcept;

    /**
     * @brief Overload the inequality operator (!=).
     *
     * Compares the stored value with another value.
     *
     * @param rhs The value to compare against.
     * @return bool True if the stored value does not equal the given value, false otherwise.
     */
    bool operator!=(const T& rhs) const noexcept;

    /**
     * @brief Overload the greater-than operator (>).
     *
     * Compares the stored value with another value.
     *
     * @param rhs The value to compare against.
     * @return bool True if the stored value is greater than the given value, false otherwise.
     */
    bool operator>(const T& rhs) const noexcept;

    /**
     * @brief Overload the less-than operator (<).
     *
     * Compares the stored value with another value.
     *
     * @param rhs The value to compare against.
     * @return bool True if the stored value is less than the given value, false otherwise.
     */
    bool operator<(const T& rhs) const noexcept;

    /**
     * @brief Overload the greater-than-or-equal operator (>=).
     *
     * Compares the stored value with another value.
     *
     * @param rhs The value to compare against.
     * @return bool True if the stored value is greater than or equal to the given value, false otherwise.
     */
    bool operator>=(const T& rhs) const noexcept;

    /**
     * @brief Overload the less-than-or-equal operator (<=).
     *
     * Compares the stored value with another value.
     *
     * @param rhs The value to compare against.
     * @return bool True if the stored value is less than or equal to the given value, false otherwise.
     */
    bool operator<=(const T& rhs) const noexcept;

    /**
     * @brief Overload the multiplication operator (*).
     *
     * Multiplies the stored value with another value of the same type.
     *
     * @param rhs The value to multiply with.
     * @return TimestampedVariable A new object containing the result.
     */
    TimestampedVariable operator*(const T& rhs) const noexcept;

    /**
     * @brief Overload the division operator (/).
     *
     * Divides the stored value by another value of the same type.
     *
     * @param rhs The value to divide by.
     * @return TimestampedVariable A new object containing the result.
     */
    TimestampedVariable operator/(const T& rhs) const noexcept;

    /**
     * @brief Overload the addition operator (+).
     *
     * Adds the stored value to another value of the same type.
     *
     * @param rhs The value to add to the stored value.
     * @return TimestampedVariable A new object containing the result.
     */
    TimestampedVariable operator+(const T& rhs) const noexcept;

    /**
     * @brief Overload the subtraction operator (-).
     *
     * Subtracts another value of the same type from the stored value.
     *
     * @param rhs The value to subtract from the stored value.
     * @return TimestampedVariable A new object containing the result.
     */
    TimestampedVariable operator-(const T& rhs) const noexcept;

    /**
     * @brief Overload of the += operator.
     *
     * Adds a value to the stored value.
     *
     * @param rhs The value to add.
     * @return Reference to the updated object.
     */
    TimestampedVariable<T>& operator+=(const T& rhs) noexcept;

    /**
     * @brief Overload of the -= operator.
     *
     * Subtracts a value from the stored value.
     *
     * @param rhs The value to subtract.
     * @return Reference to the updated object.
     */
    TimestampedVariable<T>& operator-=(const T& rhs) noexcept;

    /**
     * @brief Overload of the *= operator.
     *
     * Multiplies the stored value by another value.
     *
     * @param rhs The value to multiply by.
     * @return Reference to the updated object.
     */
    TimestampedVariable<T>& operator*=(const T& rhs) noexcept;

    /**
     * @brief Overload of the /= operator.
     *
     * Divides the stored value by another value.
     *
     * @param rhs The value to divide by.
     * @return Reference to the updated object.
     */
    TimestampedVariable<T>& operator/=(const T& rhs) noexcept;

	static constexpr TimestampedVariable UnusedValue = {0, 0};

private:
    static bool IsZero(T val) {
        const T epsilon = 1e-6f;
        return std::fabs(val) < epsilon;
    }

};

template <typename T>
TimestampedVariable<T>::TimestampedVariable() noexcept : value(T()), timestamp(GetElapsedTimeMsec()) {}

template <typename T>
TimestampedVariable<T>::TimestampedVariable(const T& initialValue) noexcept : value(initialValue), timestamp(GetElapsedTimeMsec()) {}

template <typename T>
void TimestampedVariable<T>::SetValue(const T& newValue)  noexcept{
    value = newValue;
    timestamp = GetElapsedTimeMsec();
}

template <typename T>
T TimestampedVariable<T>::GetValue() const noexcept {
    return value;
}

template <typename T>
uint32_t TimestampedVariable<T>::GetTimestamp() const noexcept {
    return timestamp;
}

///=========================================================================//
//// Overloaded Operators
///=========================================================================//
template <typename T>
T& TimestampedVariable<T>::GetRef() noexcept {
    return value;
}

///=========================================================================//
//// Overloaded Operators
///=========================================================================//
template <typename T>
uint32_t& TimestampedVariable<T>::GetTimestampRef() noexcept {
    return timestamp;
}

template <typename T>
TimestampedVariable<T>::operator T() const noexcept {
    return GetValue();
}

template <typename T>
TimestampedVariable<T>& TimestampedVariable<T>::operator=(const T& newValue) noexcept {
    SetValue(newValue);
    return *this;
}

template <typename T>
TimestampedVariable<T>& TimestampedVariable<T>::operator=(const TimestampedVariable& other) noexcept {
    /// Protect against self-assignment
    if (this != &other) {
        value = other.value;
        timestamp = other.timestamp;
    }
    return *this;
}

template <typename T>
bool TimestampedVariable<T>::operator==(const T& rhs) const noexcept {
    return value == rhs;
}

template <typename T>
bool TimestampedVariable<T>::operator==(const TimestampedVariable& other) const noexcept {
    return ((value == other.value) && (timestamp == other.timestamp));
}

template <typename T>
bool TimestampedVariable<T>::operator!=(const T& rhs) const  noexcept{
    return value != rhs;
}

template <typename T>
bool TimestampedVariable<T>::operator>(const T& rhs) const  noexcept{
    return value > rhs;
}

template <typename T>
bool TimestampedVariable<T>::operator<(const T& rhs) const  noexcept{
    return value < rhs;
}

template <typename T>
bool TimestampedVariable<T>::operator>=(const T& rhs) const  noexcept{
    return value >= rhs;
}

template <typename T>
bool TimestampedVariable<T>::operator<=(const T& rhs) const noexcept {
    return value <= rhs;
}

template <typename T>
TimestampedVariable<T> TimestampedVariable<T>::operator*(const T& rhs) const  noexcept{
    return TimestampedVariable(value * rhs);
}

template <typename T>
TimestampedVariable<T> TimestampedVariable<T>::operator/(const T& rhs) const  noexcept{

    if (!IsZero(rhs)) {  /// guard against division by zero
        return TimestampedVariable(value / rhs);
    }
    /// Handle division by zero as needed
    return std::numeric_limits<T>::max();
}

template <typename T>
TimestampedVariable<T> TimestampedVariable<T>::operator+(const T& rhs) const noexcept {
    return TimestampedVariable(value + rhs);
}

template <typename T>
TimestampedVariable<T> TimestampedVariable<T>::operator-(const T& rhs) const  noexcept{
    return TimestampedVariable(value - rhs);
}

template <typename T>
TimestampedVariable<T>& TimestampedVariable<T>::operator+=(const T& rhs) noexcept {
    value += rhs;
    return *this;
}

template <typename T>
TimestampedVariable<T>& TimestampedVariable<T>::operator-=(const T& rhs) noexcept {
    value -= rhs;
    return *this;
}

template <typename T>
TimestampedVariable<T>& TimestampedVariable<T>::operator*=(const T& rhs) noexcept {
    value *= rhs;
    return *this;
}

template <typename T>
TimestampedVariable<T>& TimestampedVariable<T>::operator/=(const T& rhs) noexcept {
    if (!IsZero(rhs)) {  // guard against division by zero
        value /= rhs;
    }
    /// Handle division by zero as needed
    return std::numeric_limits<T>::max();
}


#endif /// TIMESTAMPED_VARIABLE_H
