/**
 *
 * Nebula Tech Corporation
 *
 * Copyright © 2023 Nebula Tech Corporation. All Rights Reserved.
 * This file is part of HardFOC and is licensed under the GNU General Public License v3.0 or later.
 *
 */

#ifndef UTILITIES_COMMON_ENUMARRAY_H_
#define UTILITIES_COMMON_ENUMARRAY_H_

#include <array>

/**
 * @brief A generic array indexed by an enumeration type.
 *
 * This class allows accessing elements using enum values as indices.
 * It provides both mutable and const access to the stored objects.
 *
 * @tparam EnumType The enumeration type used as indices.
 * @tparam EnumObjectType The type of objects stored in the array.
 * @tparam Size The size of the array.
 */
template <typename EnumType, typename EnumObjectType, size_t Size>
class EnumArray {
public:
    /**
     * @brief Default constructor initializes the array.
     */
    EnumArray() : data_{} {}

	/**
	 * @brief Access elements ny enum value
	 * @param index Array index
	 * @return Object stored.
	 */
	EnumObjectType operator[](EnumType index) {
		return data_[static_cast<size_t>(index)];
	}

    /**
     * @brief Function call operator for mutable access.
     *
     * @param index Array index (enum value).
     * @return Reference to the object stored at the specified index.
     */
    EnumObjectType& operator()(EnumType index) {
        return data_[static_cast<size_t>(index)];
    }

    /**
     * @brief Assignment operator for assigning an EnumObjectType directly to all elements.
     *
     * @param value The value to assign to all elements in the array.
     * @return Reference to the current object after assignment.
     */
    EnumArray& operator=(const EnumObjectType& value) {
        for (size_t i = 0; i < Size; ++i) {
            data_[i] = value;
        }
        return *this;
    }

    /**
     * @brief Assignment operator for assigning a specific value to a specific index.
     *
     * @param keyValue A pair containing the enum index and the value to assign.
     * @return Reference to the current object after assignment.
     */
    EnumArray& operator=(std::pair<EnumType, EnumObjectType> keyValue) {
        data_[static_cast<size_t>(keyValue.first)] = keyValue.second;
        return *this;
    }

    /**
     * @brief Function call operator for const access.
     *
     * @param index Array index (enum value).
     * @return Const reference to the object stored at the specified index.
     */
    const EnumObjectType& operator()(EnumType index) const {
    	return data_[static_cast<size_t>(index)];
    }

    /**
     * @brief Assignment operator.
     *
     * @param other Another EnumArray object to assign from.
     * @return Reference to the current object after assignment.
     */
    EnumArray& operator=(const EnumArray& other) {
        if (this != &other) {
            data_ = other.data_;
        }
        return *this;
    }

    /**
     * @brief Equality operator.
     *
     * @param other Another EnumArray object to compare with.
     * @return True if the two EnumArray objects are equal, false otherwise.
     */
    bool operator==(const EnumArray& other) const {
        return data_ == other.data_;
    }

    /**
     * @brief Inequality operator.
     *
     * @param other Another EnumArray object to compare with.
     * @return True if the two EnumArray objects are not equal, false otherwise.
     */
    bool operator!=(const EnumArray& other) const {
        return !(*this == other);
    }

    // Forwarding methods
    bool empty() const { return data_.empty(); }
    size_t size() const { return data_.size(); }
    EnumObjectType& front() { return data_.front(); }
    const EnumObjectType& front() const { return data_.front(); }
    EnumObjectType& back() { return data_.back(); }
    const EnumObjectType& back() const { return data_.back(); }
    void fill(const EnumObjectType& value) { data_.fill(value); }
    void swap(EnumArray& other) noexcept { data_.swap(other.data_); }

private:
    std::array<EnumObjectType, Size> data_;
};


#endif /* UTILITIES_COMMON_ENUMARRAY_H_ */
