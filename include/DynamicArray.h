/**
 * @file DynamicArray.h
 * @brief Lightweight container with a fixed maximum capacity.
 *
 * Nebula Tech Corporation
 *
 * Copyright © 2024 Nebula Tech Corporation. All Rights Reserved.
 * This file is part of HardFOC and is licensed under the GNU General Public
 * License v3.0 or later.
 *
 * This header defines a very small dynamic array implementation that avoids
 * dynamic memory allocation.  The array stores up to `MaxCount` elements of
 * `DataType` and provides basic append, insert and remove operations along with
 * a simple random access iterator.
 */

#ifndef UTILITIES_COMMON_DYNAMICARRAY_H_
#define UTILITIES_COMMON_DYNAMICARRAY_H_

#include <array>
#include <initializer_list>
#include <algorithm>
#include <iterator>
#include <cstddef>
#include <functional>

template <typename DataType, uint8_t MaxCount>
class DynamicArray
{
public:

    DynamicArray() : data(), count(0) {}

    DynamicArray(const std::initializer_list<DataType> targets) noexcept :
		data(),
		count(0)
    {
    	count = std::min(static_cast<uint8_t>(targets.size()), MaxCount);  /// Only copy as many elements as will fit
        std::copy(targets.begin(), targets.begin() + count, data.begin());
    }

    /// Copy constructor
    DynamicArray(const DynamicArray& other) noexcept :
        data(),
        count(other.count)
    {
        std::copy(other.data.begin(), other.data.begin() + count, data.begin());
    }

    /// Copy assignment operator
    DynamicArray& operator=(const DynamicArray& other) noexcept
    {
        if (this != &other) {
            count = other.count;
            std::copy(other.data.begin(), other.data.begin() + count, data.begin());
        }
        return *this;
    }

    bool Append(const DataType& item) {
        if (count < MaxCount) {
            data[count++] = item;
            return true;
        }
        return false;
    }

    bool Remove(std::function<bool(const DataType&)>& condition) {
        auto it = std::find_if(begin(), end(), condition);
        if (it != end()) {
            std::swap(*it, data[--count]);
            return true;
        }
        return false;
    }

    bool Insert(size_t index, const DataType& item) {
        if (index <= count && count < MaxCount) {
            std::rotate(data.begin() + index, data.begin() + count, data.end());
            data[index] = item;
            ++count;
            return true;
        }
        return false;
    }

    void ClearAll() {
        count = 0;
    }

    DataType& operator[](const size_t& index) {
        return data[index];
    }

    const DataType& operator[](const size_t& index) const {
        return data[index];
    }

	//=============================================================//
	/// RANDOM ACCESS ITERATOR.
	//=============================================================//

    /**
     * @brief Random access iterator for the MultiRail class.
     *
     * This iterator allows direct access to any element in the readings array.
     * It supports all the operations required for a random access iterator,
     * including moving forwards and backwards, direct access to any element,
     * and comparisons between iterators.
     */
    struct iterator
    {
    	/// The category of the iterator. Random access iterators support a superset of the operations of bidirectional iterators and forward iterators.
    	using iterator_category = std::random_access_iterator_tag;
    	/// A type that provides the difference between two iterators.
    	using difference_type   = std::ptrdiff_t;
    	/// The type of the elements pointed to by the iterator.
    	using value_type        = DataType;
    	/// The type of a pointer to an element pointed to by the iterator.
    	using pointer           = value_type*;
    	/// The type of a reference to an element pointed to by the iterator.
    	using reference         = value_type&;

    	/**
    	 * @brief Construct a new iterator object.
    	 *
    	 * @param ptr Pointer to the current reading.
    	 */
    	iterator(pointer ptr) noexcept : currentReading(ptr) {}

    	/// Copy constructor. Constructs the iterator with the copy of the contents of other.
    	iterator(const iterator& copy ) noexcept = default;

    	/// Copy assignment operator. Replaces the contents with a copy of the contents of other.
    	iterator& operator = (const iterator& copy ) noexcept = default;

    	/// Destructor. Destroys the iterator.
    	~iterator() = default;

    	/// Dereference operator. Gives the value pointed to by the iterator.
    	reference operator*() const { return *currentReading; }

    	/// Arrow operator. Gives a pointer to the value pointed to by the iterator.
    	pointer operator->() const { return currentReading; }

    	/// Prefix increment operator. Advances the iterator to the next element and returns an iterator to the new current element.
    	iterator& operator++() { currentReading++; return *this; }

    	/// Postfix increment operator. Advances the iterator to the next element and returns an iterator to the element before the increment.
    	iterator operator++(int) { iterator tmp = *this; ++(*this); return tmp; }

    	/// Prefix decrement operator. Moves the iterator to the previous element and returns an iterator to the new current element.
    	iterator& operator--() { currentReading--; return *this; }

    	/// Postfix decrement operator. Moves the iterator to the previous element and returns an iterator to the element before the decrement.
    	iterator operator--(int) { iterator tmp = *this; --(*this); return tmp; }

    	/// Addition operator. Returns an iterator that is n positions ahead of the current iterator.
    	iterator operator+(difference_type n) const { return iterator(currentReading + n); }

    	/// Subtraction operator. Returns an iterator that is n positions behind the current iterator.
    	iterator operator-(difference_type n) const { return iterator(currentReading - n); }

    	/// Difference operator. Returns the distance between two iterators.
    	difference_type operator-(const iterator& other) const { return currentReading - other.currentReading; }

    	/// Array subscript operator. Returns the n-th element from the current position.
    	reference operator[](difference_type n) const { return *(currentReading + n); }

    	/// Less than operator. Checks if the current iterator points to an element before the other iterator.
    	bool operator<(const iterator& other) const { return currentReading < other.currentReading; }

    	/// Less than or equal to operator. Checks if the current iterator points to an element before or at the same position as the other iterator.
    	bool operator<=(const iterator& other) const { return currentReading <= other.currentReading; }

    	/// Greater than operator. Checks if the current iterator points to an element after the other iterator.
    	bool operator>(const iterator& other) const { return currentReading > other.currentReading; }

    	/// Greater than or equal to operator. Checks if the current iterator points to an element after or at the same position as the other iterator.
    	bool operator>=(const iterator& other) const { return currentReading >= other.currentReading; }

    	/// Equality operator. Checks if both iterators point to the same element.
    	bool operator==(const iterator& other) const { return currentReading == other.currentReading; }

    	/// Inequality operator. Checks if the iterators point to different elements.
    	bool operator!=(const iterator& other) const { return !operator==(other); }

        /**
         * @brief Compound addition (plus-equal) operator.
         *
         * Advances the iterator by n positions.
         *
         * @param n The number of positions to advance the iterator by.
         * @return A reference to the updated iterator.
         */
        iterator& operator+=(difference_type n) { currentReading += n; return *this; }

        /**
         * @brief Compound subtraction (minus-equal) operator.
         *
         * Moves the iterator back by n positions.
         *
         * @param n The number of positions to move the iterator back by.
         * @return A reference to the updated iterator.
         */
        iterator& operator-=(difference_type n) { currentReading -= n; return *this; }

    private:
    	/// Pointer to the current reading.
    	pointer currentReading;
    };

    // Random access iterator for const objects
    struct const_iterator {
        using iterator_category = std::random_access_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = const DataType;
        using pointer = const value_type*;
        using reference = const value_type&;

        const_iterator(pointer ptr) noexcept : currentReading(ptr) {}

        const_iterator(const const_iterator& copy) noexcept = default;
        const_iterator& operator=(const const_iterator& copy) noexcept = default;
        ~const_iterator() = default;

        reference operator*() const { return *currentReading; }
        pointer operator->() const { return currentReading; }

        const_iterator& operator++() { currentReading++; return *this; }
        const_iterator operator++(int) { const_iterator tmp = *this; ++(*this); return tmp; }
        const_iterator& operator--() { currentReading--; return *this; }
        const_iterator operator--(int) { const_iterator tmp = *this; --(*this); return tmp; }

        const_iterator operator+(difference_type n) const { return const_iterator(currentReading + n); }
        const_iterator operator-(difference_type n) const { return const_iterator(currentReading - n); }
        difference_type operator-(const const_iterator& other) const { return currentReading - other.currentReading; }

        reference operator[](difference_type n) const { return *(currentReading + n); }

        bool operator<(const const_iterator& other) const { return currentReading < other.currentReading; }
        bool operator<=(const const_iterator& other) const { return currentReading <= other.currentReading; }
        bool operator>(const const_iterator& other) const { return currentReading > other.currentReading; }
        bool operator>=(const const_iterator& other) const { return currentReading >= other.currentReading; }
        bool operator==(const const_iterator& other) const { return currentReading == other.currentReading; }
        bool operator!=(const const_iterator& other) const { return !operator==(other); }

        const_iterator& operator+=(difference_type n) { currentReading += n; return *this; }
        const_iterator& operator-=(difference_type n) { currentReading -= n; return *this; }

    private:
        pointer currentReading;
    };

    /**
     * @brief Returns an iterator to the beginning of the array.
     *
     * This function provides a non-const iterator to the first element of the array.
     *
     * @return iterator to the first element.
     */
    iterator begin() noexcept { return iterator(data.data()); }

    /**
     * @brief Returns an iterator to the end of the array.
     *
     * This function provides a non-const iterator to the element following the last element of the array.
     *
     * @return iterator to the element following the last element.
     */
    iterator end() noexcept { return iterator(data.data() + count); }


    /**
     * @brief Returns a const iterator to the beginning of the array.
     *
     * This function provides a const iterator to the first element of the array.
     *
     * @return const_iterator to the first element.
     */
    const_iterator begin() const noexcept { return const_iterator(data.data()); }

    /**
     * @brief Returns a const iterator to the end of the array.
     *
     * This function provides a const iterator to the element following the last element of the array.
     *
     * @return const_iterator to the element following the last element.
     */
    const_iterator end() const noexcept { return const_iterator(data.data() + count); }


    /**
     * @brief Returns a const iterator to the beginning of the array.
     *
     * This function provides a const iterator to the first element of the array.
     * It is equivalent to the const version of begin().
     *
     * @return const_iterator to the first element.
     */
    const_iterator cbegin() const noexcept { return const_iterator(data.data()); }

    /**
     * @brief Returns a const iterator to the end of the array.
     *
     * This function provides a const iterator to the element following the last element of the array.
     * It is equivalent to the const version of end().
     *
     * @return const_iterator to the element following the last element.
     */
    const_iterator cend() const noexcept { return const_iterator(data.data() + count); }


    /**
     * @brief Returns the number of elements in the array.
     *
     * This function returns the current number of elements stored in the array.
     *
     * @return The number of elements in the array.
     */
    size_t size() const noexcept { return count; }

    /**
     * @brief Returns the maximum capacity of the array.
     *
     * This function returns the maximum number of elements that the array can hold.
     *
     * @return The maximum capacity of the array.
     */
    size_t capacity() const noexcept { return MaxCount; }

    /**
     * @brief Checks if the array is empty.
     *
     * This function checks whether the array contains any elements.
     *
     * @return True if the array is empty, false otherwise.
     */
    bool empty() const noexcept { return count == 0; }


private:
    std::array<DataType, MaxCount> data;
    uint8_t count;
};


#endif /* UTILITIES_COMMON_DYNAMICARRAY_H_ */
