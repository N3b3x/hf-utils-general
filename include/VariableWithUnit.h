/**
 *
 * Nebula Tech Corporation
 *
 * Copyright © 2025 Nebula Tech Corporation. All Rights Reserved.
 * This file is part of HardFOC and is licensed under the GNU General Public License v3.0 or later.
 *
 */

#ifndef UTILITIES_COMMON_VARIABLEWITHUNIT_H_
#define UTILITIES_COMMON_VARIABLEWITHUNIT_H_

/**
 * @class VariableWithUnit
 * @tparam T The type of the value to be stored.
 * @tparam U The type of the unit to be associated with the value.
 * @brief A class template that stores a value of type T and an associated unit of type U.
 *
 * This class provides a way to associate a unit with a variable.
 */
template <typename T, typename U>
class VariableWithUnit {
private:
    T value;  ///< The value of the variable.
    U unit;   ///< The unit associated with the value.

public:
    /**
     * @brief Default constructor.
     *
     * Initializes the value to its default and the unit to its default.
     */
    VariableWithUnit() noexcept : value(T()), unit(U()) {}

    /**
     * @brief Default constructor.
     *
     * Initializes the value to its default and the unit to its default.
     */
    VariableWithUnit(const T& initialValue) noexcept : value(initialValue), unit(U()) {}

    /**
     * @brief Overloaded constructor.
     *
     * Initializes the value and the unit to the given parameters.
     *
     * @param initialValue The initial value for the variable.
     * @param initialUnit The initial unit for the variable.
     */
    VariableWithUnit(const T& initialValue, const U& initialUnit) noexcept : value(initialValue), unit(initialUnit) {}

    /**
     * @brief Copy constructor.
     *
     * Creates a new VariableWithUnit as a copy of the given VariableWithUnit.
     *
     * @param other The VariableWithUnit to copy.
     */
    VariableWithUnit(const VariableWithUnit& other) noexcept = default;

    /**
     * @brief Setter function to update the value.
     *
     * Sets a new value for the variable.
     *
     * @param newValue The new value to set.
     */
    void SetValue(const T& newValue) noexcept {
        value = newValue;
    }

    /**
     * @brief Getter function for the value.
     *
     * @return T The current value of the variable.
     */
    T GetValue() const noexcept {
        return value;
    }

    /**
     * @brief Setter function to update the unit.
     *
     * Sets a new unit for the variable.
     *
     * @param newUnit The new unit to set.
     */
    void SetUnit(const U& newUnit) noexcept {
        unit = newUnit;
    }

    /**
     * @brief Getter function for the unit.
     *
     * @return U The current unit of the variable.
     */
    U GetUnit() const noexcept {
        return unit;
    }

    /**
     * @brief Function to check if the current unit matches the provided unit.
     *
     * This function compares the current unit of the variable with the provided unit.
     *
     * @param checkUnit The unit to compare with the current unit.
     * @return Returns true if the current unit matches the provided unit, false otherwise.
     */
    bool IsUnit(const U& checkUnit) noexcept {
        return (unit == checkUnit);
    }

    // Overload the assignment operator
    VariableWithUnit& operator=(const VariableWithUnit& other) {
        if (this != &other) {
            value = other.value;
            unit = other.unit;
        }
        return *this;
    }

    // Overload the equality operator
    bool operator==(const VariableWithUnit& other) const {
        return value == other.value && unit == other.unit;
    }

    // Overload the inequality operator
    bool operator!=(const VariableWithUnit& other) const {
    	return !(*this == other);
    }

    // Overload the assignment operator for type T
    VariableWithUnit& operator=(const T& newValue) {
    	value = newValue;
    	return *this;
    }

    // Overload the type-casting operator for type T
    operator T() const {
    	return value;
    }

    // Overload the type-casting operator for type U
    operator U() const {
    	return unit;
    }

    // Overload the addition operator
    VariableWithUnit operator+(const VariableWithUnit& other) const {
    	assert(unit == other.unit);
    	return VariableWithUnit(value + other.value, unit);
    }

    // Overload the subtraction operator
    VariableWithUnit operator-(const VariableWithUnit& other) const {
    	assert(unit == other.unit);
    	return VariableWithUnit(value - other.value, unit);
    }

    // Overload the multiplication operator
    VariableWithUnit operator*(const T& scalar) const {
    	return VariableWithUnit(value * scalar, unit);
    }

    // Overload the division operator
    VariableWithUnit operator/(const T& scalar) const {
    	assert(scalar != 0);
    	return VariableWithUnit(value / scalar, unit);
    }

    /**
     * @brief Overload the less than operator for comparison with int.
     * @param other The int to compare with.
     * @return True if the value of this object is less than the other int, false otherwise.
     */
    bool operator<(const int& other) const {
        return value < static_cast<T>(other);
    }

    /**
     * @brief Overload the less than or equal to operator for comparison with int.
     * @param other The int to compare with.
     * @return True if the value of this object is less than or equal to the other int, false otherwise.
     */
    bool operator<=(const int& other) const {
        return value <= static_cast<T>(other);
    }

    /**
     * @brief Overload the greater than operator for comparison with int.
     * @param other The int to compare with.
     * @return True if the value of this object is greater than the other int, false otherwise.
     */
    bool operator>(const int& other) const {
        return value > static_cast<T>(other);
    }

    /**
     * @brief Overload the greater than or equal to operator for comparison with int.
     * @param other The int to compare with.
     * @return True if the value of this object is greater than or equal to the other int, false otherwise.
     */
    bool operator>=(const int& other) const {
        return value >= static_cast<T>(other);
    }
};




#endif /* UTILITIES_COMMON_VARIABLEWITHUNIT_H_ */
