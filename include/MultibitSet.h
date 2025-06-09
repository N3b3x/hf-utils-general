/**
 * @file MultibitSet.h
 * @brief Small wrapper around std::bitset for multi-bit entries.
 *
 * Nebula Tech Corporation
 *
 * Copyright © 2023 Nebula Tech Corporation. All Rights Reserved.
 * This file is part of HardFOC and is licensed under the GNU General Public
 * License v3.0 or later.
 *
 * The MultibitSet template manages a bitset partitioned into logical entries of
 * a fixed size.  Each element stores `BitsPerEntry` bits and the class provides
 * helpers to set, clear and fetch those fields by index.
 */

#ifndef UTILITIES_COMMON_MULTIBITSET_H
#define UTILITIES_COMMON_MULTIBITSET_H

#include <bitset>
#include <cstdint>

/**
 * @class MultiBitSet
 * @brief This class is used to a multi-bit value into a std::bitset
 * @tparam BitsPerEntry The number of bits per entry
 * @tparam EntryCount   The number of entries.
 */
template < uint8_t BitsPerEntry, uint16_t EntryCount> class MultibitSet
{

public:
	/**
	 * @brief Constructor for MultiBitSet.
	 * @param initialValueArg - specified the initial bit sequence to save in each cell for un-itialized state.
	 */
	MultibitSet( uint8_t initialValueArg = 0 ) noexcept :
		initialValue(initialValueArg),
		bitset()
	{
	   erase();
	}

	MultibitSet( const MultibitSet& copy ) noexcept = default;

	MultibitSet& operator = ( const MultibitSet& copy ) noexcept = default;

	~MultibitSet() = default;

	/**
	 * @brief Returns the number of MultibitSet values
	 */
	uint16_t size() const noexcept
	{
		return EntryCount;
	}

	/**
	  * @brief Sets the elements of the multi-set to their un-initalized (default) value
	 */
	void erase() noexcept
	{
		for( uint16_t valueIndex = 0; valueIndex < size(); ++valueIndex )
		{
			clear( valueIndex);
		}
	}

	/**
      * @brief Sets the specified elements of the multi-set to a designated value (if the index is within range)
      * @param valueIndex - index from 0 to EntryCount to update
      * @param value - value to store ( only least BitsPerEntry are saved )
	  */
    void set( uint16_t valueIndex, uint8_t value ) noexcept
    {
    	if( valueIndex < EntryCount )
    	{

    		volatile const uint16_t firstBit = valueIndex * BitsPerEntry; // BJR 5: 9
    		volatile const uint16_t lastBit = firstBit + BitsPerEntry - 1;

			uint8_t bitMask = 1 << (BitsPerEntry-1);
			for( uint16_t bitsetIndex = firstBit; bitsetIndex <= lastBit; ++bitsetIndex )
			{
				bitset[bitsetIndex] = value & bitMask;     // Populate the bit
				bitMask >>= 1;
			}
    	}
    }

    /**
	  * @brief Sets the specified element of the multi-set to the un-initalized (default) value (if the index is within range)
	  * @param valueIndex - index from 0 to EntryCount to update
	  * @param value - value to store ( only least BitsPerEntry are saved )
	  */

    void clear( uint16_t valueIndex) noexcept
    {
    	set(valueIndex, initialValue);
    }

    /**
  	  * @brief Retrieves the specified element of the multi-set.
  	  * @param valueIndex - index from 0 to EntryCount to update
  	  * @returns Value save at the index, or the  un-initalized (default) value when the index is out of range
  	  */


     uint8_t get( uint16_t valueIndex) const noexcept
     {
		if( valueIndex < EntryCount )
		{
			uint8_t value = 0;
			const uint16_t firstBit = valueIndex * BitsPerEntry; // BJR 5: 9
			const uint16_t lastBit = firstBit + BitsPerEntry -1;

			for( uint16_t bitsetIndex = firstBit; bitsetIndex <= lastBit; ++bitsetIndex )
			{
				if( bitset[bitsetIndex] )    // Populate the bit
				{
					value += 1;
				}
				if( bitsetIndex < lastBit )
				{
					value <<= 1;
				}
			}
			return value;
		}
		else
		{
			return initialValue;
		}
     }

private:

     uint8_t initialValue;
	 std::bitset<BitsPerEntry * EntryCount> bitset;
};


#endif /* UTILITIES_COMMON_MULTIBITSET_H */
