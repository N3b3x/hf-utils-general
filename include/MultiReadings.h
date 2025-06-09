/**
 * Nebula Tech Corporation
 *
 * Copyright © 2024 Nebula Tech Corporation. All Rights Reserved.
 * This file is part of HardFOC and is licensed under the GNU General Public License v3.0 or later.
 */

#ifndef UTILITIES_COMMON_MULTIREADINGS_H_
#define UTILITIES_COMMON_MULTIREADINGS_H_

#include <array>
#include <initializer_list>
#include <algorithm>
#include <iterator>
#include <cstddef>

#include "platform_compat.h"

#include <UTILITIES/common/CommonIDs.h>
#include <UTILITIES/common/ThingsToString.h>

#include <HAL/component_handlers/ConsolePort.h>

/**
 * @brief A class template to manage multiple sensor readings.
 *
 * This class template provides a way to manage multiple sensor readings. It allows you to append readings, get averages, and reset readings.
 *
 * @tparam IdentifierType The type of the identifier for the readings.
 * @tparam DataType The data type of the sum of readings.
 * @tparam MaxIdentifiersCount Maximum number of identifiers that can be handled.
 * @tparam ExtraType An optional type that the user can specify. Defaults to void.
 */
template <typename IdentifierType, typename DataType, uint8_t MaxIdentifiersCount, typename ExtraType = void>
class MultiReadings
{
public:
	//=============================================================//
	/// CONSTANTS/VARIABLES/STRUCTS
	//=============================================================//

    /**
     * @brief Structure to capture the sensor input, number of readings and the summed counts for the channel.
     *
     * This structure represents a reading from a sensor. It includes the identifier of the sensor, the number of samples per reading, the current number of readings, and the sum of the readings.
     *
     * If ExtraType is not void, it also includes an extraSpec member of type ExtraType and a GetExtraSpec method to retrieve the value of extraSpec.
     */
    struct Reading
    {
    	Reading() : identifier(), numOfSamplesPerReading(0), currentNumberOfReadings(0), sumOfReadings(), extraSpec() {}

    	//============//
    	/// SPECS
    	//============//
        IdentifierType identifier;      		///< Channel of identified type being sampled
        uint8_t numOfSamplesPerReading; 		///< Number of samples each reading of the sensor needs to do and average.

    	//============//
    	/// DATA
    	//============//
        uint32_t currentNumberOfReadings;		///< Number of readings currently added to sum
        DataType  sumOfReadings;        		///< Sum of readings

        /**
         * @brief Append a reading to the sum and increment the number of readings.
         *
         * @param reading The reading to append.
         */
        void Append( DataType reading ) noexcept
        {
            sumOfReadings += reading;
            ++currentNumberOfReadings;
        }

        /**
         * @brief Append a reading to the sum and increment the number of readings.
         *
         * @param reading The reading to append.
         */
        DataType GetAverage() noexcept
        {
            return (static_cast<DataType>(static_cast<float>(sumOfReadings) / static_cast<float>(currentNumberOfReadings)));
        }

        /**
         * @brief Get the number of samples per reading.
         *
         * @return The number of samples per reading.
         */
        uint8_t GetNumOfSamplesPerReading() const noexcept
        {
            return numOfSamplesPerReading;
        }

    	//============================//
    	/// EXTRA SPEC - USER DEFINED
    	//============================//
        /// Conditionally include the extraSpec member
        typename std::conditional<!std::is_same<ExtraType, void>::value, ExtraType, std::nullptr_t>::type extraSpec;	///< Extra specification provided by the user. Only included if ExtraType is not void.

    };

	//=============================================================//
	/// CLASS
	//=============================================================//

    using pIdentifierTypeToStringFunc = const char*(*)(const IdentifierType);
    static const char* DefaultIdentifierToString(const IdentifierType type) {
        UTIL_UNUSED(type);
        return "ChannelNameUnknown";
    }

    /**
     * @brief Default constructor. Initializes readings array and channelsCount to zero.
     */
    MultiReadings(pIdentifierTypeToStringFunc channelToStringFunc = &DefaultIdentifierToString) noexcept
        : readings(), channelsCount(0), identifierTypeToString(channelToStringFunc)
    {
        Reset();
    }


    /**
     * @brief Constructor that takes an initializer list of pairs. Each pair contains an IdentifierType object and the number of samples to do per reading.
     * It initializes the readings array with the provided sensors and sets the channelsCount accordingly.
     * This constructor is only enabled if ExtraType is void.
     *
     * @tparam T A template parameter that defaults to ExtraType.
     * @tparam std::enable_if<std::is_same<T, void>::value, int>::type A SFINAE expression that enables this function only if T is void. If T is not void, this function is removed from the class during compilation.
     * @param inputChannels The initializer list of pairs. Each pair contains an IdentifierType object and the number of samples to do per reading.
     * @param channelToStringFunc Function to convert a channel to a string. Default is DefaultIdentifierToString.
     */
    template <typename T = ExtraType,
    		typename std::enable_if<std::is_same<T, void>::value, int>::type = 0>
    MultiReadings(const std::initializer_list<std::pair<IdentifierType, uint8_t>>& inputChannels,
    		pIdentifierTypeToStringFunc channelToStringFunc = DefaultIdentifierToString) noexcept :
			readings(),
			channelsCount(0),
			identifierTypeToString(channelToStringFunc)
    {
    	for(auto iter = inputChannels.begin(); iter != inputChannels.end() && channelsCount < MaxIdentifiersCount; ++iter)
    	{
            readings[channelsCount].identifier = std::get<0>(*iter);
            readings[channelsCount].numOfSamplesPerReading = std::get<1>(*iter);
    		++channelsCount;
    	}
    }

    /**
     * @brief This is a constructor that takes an initializer list of tuples. Each tuple contains an IdentifierType object, the number of samples to do per reading, and an ExtraType object.
     * It initializes the readings array with the provided sensors and sets the channelsCount accordingly.
     * This constructor is only enabled if ExtraType is not void.
     *
     * @tparam T A template parameter that defaults to ExtraType.
     * @tparam std::enable_if<!std::is_same<T, void>::value, int>::type A SFINAE expression that enables this function only if T is not void. If T is void, this function is removed from the class during compilation.
     * @param inputChannels The initializer list of tuples. Each tuple contains an IdentifierType object, the number of samples to do per reading, and an ExtraType object.
     * @param channelToStringFunc Function to convert a channel to a string. Default is DefaultIdentifierToString.
     */
    template <typename T = ExtraType,
              typename std::enable_if<!std::is_same<T, void>::value, int>::type = 0>
    MultiReadings(const std::initializer_list<std::tuple<IdentifierType, uint8_t, ExtraType>>& inputChannels,
                  pIdentifierTypeToStringFunc channelToStringFunc = DefaultIdentifierToString) noexcept :
        readings(),
        channelsCount(0),
        identifierTypeToString(channelToStringFunc)
    {
        for(auto iter = inputChannels.begin(); iter != inputChannels.end() && channelsCount < MaxIdentifiersCount; ++iter)
        {
            readings[channelsCount].identifier = std::get<0>(*iter);
            readings[channelsCount].numOfSamplesPerReading = std::get<1>(*iter);
            readings[channelsCount].extraSpec = std::get<2>(*iter);
            ++channelsCount;
        }
    }


    /**
     * @brief Copy constructor. Creates a new object as a copy of an existing object.
     *
     * @param copy The object to copy from.
     */
    MultiReadings(const MultiReadings& copy) noexcept = default;

    /**
     * @brief Copy assignment operator. Assigns the state of one object to another object of the same type.
     *
     * @param copy The object to copy from.
     * @return A reference to *this after the copy is performed.
     */
    MultiReadings& operator=(const MultiReadings& copy) noexcept = default;


    /**
     * @brief Destructor. Called when the object goes out of scope or is explicitly deleted.
     */
    ~MultiReadings() noexcept = default;

	/**
	 * @brief Append an input sensor to the current sensor list if it hasn't reached MaxSensorCount.
	 *
	 * @param inputChannel The input sensor to append.
	 * @return true if the sensor was successfully appended, false otherwise.
	 */
	bool AppendSensor(IdentifierType identifier)
	{
		if( (channelsCount + 1U) < MaxIdentifiersCount)
		{
			readings[channelsCount++].identifier = identifier;
			return true;
		}
		return false;
	}

	/**
	 * @brief Append a reading to the specified sensor if it exists in the readings array.
	 *
	 * @param inputChannel The input sensor to append the reading to.
	 * @param reading The reading in counts.
	 * @return true if the reading was successfully appended, false otherwise.
	 */
	bool AppendReading(IdentifierType identifier, DataType reading)
	{
		auto iter = FindReading(identifier);

		if(iter != readings.end())
		{
			iter->Append(reading);
			return true;
		}
		return false;
	}

	/**
	 * @brief Retrieves the Reading struct for a given identifier.
	 *
	 * This method finds the Reading associated with the given identifier and sets the reading parameter to it.
	 * If the identifier is not found in the readings, it leaves the reading parameter unchanged.
	 *
	 * @param identifier The identifier of the reading.
	 * @param reading A reference to a Reading struct where the result will be stored.
	 * @return true if the identifier was found and the reading was set, false otherwise.
	 */
	bool GetReading(const IdentifierType& identifier, Reading& reading)
	{
		auto iter = FindReading(identifier);

		if(iter != readings.end())
		{
			reading = *iter;
			return true;
		}
		else
		{
			return false;
		}
	}

    /**
     * @brief Retrieves the average reading for the specified sensor if it exists in the readings array and has more than zero readings.
     *
     * @param inputChannel The input sensor to get the average reading for.
     * @param averageReading The average reading in counts.
     * @return true if the average reading was successfully retrieved, false otherwise.
     */
    bool GetAverage(IdentifierType identifier, DataType& averageReading) noexcept
    {
		auto iter = FindReading(identifier);

        if(iter != readings.end())
        {
            if(iter->currentNumberOfReadings > 0)
            {
                averageReading = iter->GetAverage();
                return true;
            }
            else
            {
                ConsolePort::Write("MultiReadings::GetAverage() - Zero readings for: %s.", identifierTypeToString(identifier));
            }
        }
        else
        {
            ConsolePort::Write("MultiReadings::GetAverage() - Failed to find entry for: %s.", identifierTypeToString(identifier));
        }

        return false;
    }

    /**
     * @brief Retrieves the number of samples per reading for a given identifier.
     *
     * This method finds the Reading associated with the given identifier and sets numOfSamples to its numOfSamplesPerReading.
     * If the identifier is not found in the readings, it leaves numOfSamples unchanged.
     *
     * @param identifier The identifier of the reading.
     * @param numOfSamples Reference to a variable where the number of samples per reading will be stored.
     * @return true if the identifier was found and numOfSamples was set, false otherwise.
     */
    bool GetSamplesPerReading(const IdentifierType& identifier, uint8_t& numOfSamples) const noexcept
    {
		auto iter = FindReading(identifier);

        if(iter != readings.end())
        {
            numOfSamples = iter->numOfSamplesPerReading;
            return true;
        }
        else
        {
            return false;
        }
    }

    /**
     * @brief Retrieves the number of samples per reading for a given identifier.
     *
     * This method finds the Reading associated with the given identifier and sets numOfSamples to its numOfSamplesPerReading.
     * If the identifier is not found in the readings, it leaves numOfSamples unchanged.
     *
     * @param identifier The identifier of the reading.
     * @return true if the identifier was found and numOfSamples was set, false otherwise.
     */
    template<typename T = ExtraType>
    typename std::enable_if<!std::is_same<T, void>::value, bool>::type GetExtraSpec(const IdentifierType& identifier, T& extraSpec) const noexcept
    {
        auto iter = FindReading(identifier);

        if(iter != readings.end())
        {
            extraSpec = iter->extraSpec;
            return true;
        }
        else
        {
            return false;
        }
    }


    /**
     * @brief Resets the reading for all the channels by setting currentNumberOfReadings and sumOfReadings to zero for each sensor in the readings array.
     */
    void Reset() noexcept
    {
        for(auto& entry : readings)
        {
            entry.currentNumberOfReadings = 0;
            entry.sumOfReadings = 0;
        }
    }

    /**
     * @brief Random access iterator for the MultiReadings class.
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
    	using value_type        = Reading;
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

    /**
     * @brief Returns an iterator pointing to the first element in the readings array.
     *
     * If the array is empty, the returned iterator will be equal to end().
     *
     * @return An iterator to the first element in the readings array.
     */
    iterator begin() noexcept
    {
        if(channelsCount > 0) {
            return iterator(readings.data());
        }
        else {
            return iterator(readings.data() + channelsCount);
        }
    }

    /**
     * @brief Returns an iterator referring to the past-the-end element in the readings array.
     *
     * The past-the-end element is the theoretical element that would follow the last element in the array.
     * It does not point to any element, and thus shall not be dereferenced.
     *
     * @return An iterator to the element following the last element in the readings array.
     */
    iterator end() noexcept
    {
        return iterator(readings.data() + channelsCount);
    }


    /**
     * @brief A const iterator type for the MultiReadings class.
     *
     * This iterator allows read-only access to the elements in the readings array.
     */
    using const_iterator = const iterator;

    /**
     * @brief Returns a const iterator pointing to the first element in the readings array.
     *
     * If the array is empty, the returned iterator will be equal to cend().
     *
     * @return A const iterator to the first element in the readings array.
     */
    const_iterator cbegin() const noexcept
    {
        return const_iterator(readings.data());
    }

    /**
     * @brief Returns a const iterator referring to the past-the-end element in the readings array.
     *
     * The past-the-end element is the theoretical element that would follow the last element in the array.
     * It does not point to any element, and thus shall not be dereferenced.
     *
     * @return A const iterator to the element following the last element in the readings array.
     */
    const_iterator cend() const noexcept
    {
        return const_iterator(readings.data() + channelsCount);
    }

    size_t size() const noexcept { return channelsCount; }
    size_t capacity() const noexcept { return MaxIdentifiersCount; }
    bool empty() const noexcept { return channelsCount == 0; }

    Reading& operator[](const IdentifierType& identifier) {
        auto iter = FindReading(identifier);
        if(iter != readings.end()) {
            return *iter;
        } else {
            // If the identifier is not found, append a new Reading with this identifier
            // to the readings array using the AppendSensor function.
            if(AppendSensor(identifier)) {
                // If the Reading was successfully appended, return a reference to it.
                return readings.back();
            } else {
                // If the Reading could not be appended (because MaxIdentifiersCount was reached),
                // handle the error as appropriate for your program.
                // This could involve throwing an exception, returning a default Reading,
                // logging an error message, etc.
                return readings.back();
            }
        }
    }


protected:

    /**
     * @brief Finds the reading associated with the given identifier.
     *
     * This method uses the std::find_if algorithm to search the readings array for a Reading object
     * that has the same identifier as the one provided. It uses a lambda function as a predicate
     * for the std::find_if algorithm. The lambda function checks if the identifier of each Reading
     * object in the readings array matches the provided identifier.
     *
     * @param identifier The identifier of the Reading object to find.
     * @return An iterator pointing to the found Reading object in the readings array. If no Reading
     * object with the provided identifier is found, the method returns an iterator pointing to the
     * end of the readings array.
     */
    auto FindReading(IdentifierType identifier)
    {
        auto FindPredicate = [&]( const Reading& entry) { return (entry.identifier == identifier); };
        return std::find_if(readings.begin(), readings.end(), FindPredicate);
    }

    std::array<Reading, MaxIdentifiersCount> readings; 		///< Array of Reading structures.
    uint8_t channelsCount;                       			///< Number of channels.
    pIdentifierTypeToStringFunc identifierTypeToString; 	///< Function to convert a channel to a string.
};

#endif /* UTILITIES_COMMON_MULTIREADINGS_H_ */

