/**
 *
 * Nebula Tech Corporation
 *
 * Copyright © 2023 Nebula Tech Corporation. All Rights Reserved.
 * This file is part of HardFOC and is licensed under the GNU General Public License v3.0 or later.
 *
 */

#ifndef UTILITIES_COMMON_ENUMERATEDSETSTATUS_H
#define UTILITIES_COMMON_ENUMERATEDSETSTATUS_H

#include <MultibitSet.h>
#include <Utility.h>


/**
 * @class EnumeratedSetStatus
 * @brief This class is used to index a Type enumeration along with a Status enumeration.  Each enumeration is tagged
 *         with a settable/rtrievable status value
 * @tparam BitsPerStatus The number of bits per status entry
 * @tparam BitsPerEntry The number of enumeration entries
 * @tparam EntryCount   The number of entries.
 * Internally, a std::bitset is used to store the content.
 */
template <typename EnumerationType, typename StatusType,  uint8_t BitsPerStatus, uint16_t EntryCount> class EnumeratedSetStatus
{

public:

	/**
	 * @brief Constructor for EnumeratedErrorSet.
	 * @param initialValueArg - Initial / default value for enumeration.
	 * @param enumToStringConverter - Function to convert EnumerationType to string
	 * @param statusToStringConverter - Function to convert StatusType to string
	 */
	EnumeratedSetStatus( StatusType defaultValue,
						const char* (*enumToStringConverter)( EnumerationType ) = nullptr,
			            const char* (*statusToStringConverter)( StatusType ) = nullptr
						 ) noexcept :
		enumToString( enumToStringConverter ),
		statusToString( statusToStringConverter ),
		collection( std::to_underlying(defaultValue) )
	{
		/// No code at this time
	}

	EnumeratedSetStatus( const EnumeratedSetStatus& copy ) noexcept = default;

	EnumeratedSetStatus& operator = ( const EnumeratedSetStatus& copy ) noexcept = default;

	~EnumeratedSetStatus() = default;

	/**
	 * @brief Returns the number of MultibitSet values
	 */
	uint16_t size() const noexcept
	{
		return collection.size();
	}

	/**
	  * @brief Sets all elements of the collection to the default value
	 */
	void Erase() noexcept
	{
		collection.erase();
	}

	/**
      * @brief Sets the specified elements of the collection to a designated value (if the index is within range)
      * @param enumeration - Enumeration index from 0 to EntryCount to update
      * @param status - Status value to associate with the entry
	  */
    void Set(EnumerationType enumeration, StatusType status) noexcept
    {
    	collection.set( std::to_underlying(enumeration), std::to_underlying( status ));
    }

    void SetAll( StatusType status) noexcept
    {
    	for ( uint16_t entryIndex = 0; entryIndex < collection.size(); ++entryIndex )
    	{
    		collection.set( entryIndex, std::to_underlying(status) );
    	}
    }

    /**
	  * @brief Retrieves the specified elements of the collection
	  * @param enumeration - Enumeration index from 0 to EntryCount to update
	  * @returns - Status value to associated with the entry.  THe default value is returned if the index is out of range
	  */
    StatusType Get(EnumerationType enumeration) const noexcept
    {
    	uint8_t value = collection.get(std::to_underlying(enumeration));
    	return StatusType( value );
    }

    /**
   	  * @brief Identifies if any elements of the collection as the specified status
   	  * @param status - Status value to be checked.
   	  * @returns - true if any element has the Status value, false otherwise.
   	  */
    bool IsAny(StatusType status) const noexcept
    {
    	for ( uint16_t errorIndex = 0; errorIndex < collection.size(); ++errorIndex )
		{
			uint8_t value = collection.get(errorIndex);
			if( value == std::to_underlying( status ))
			{
				return true;
			}
		}
		return false;
    }

    /**
     * @brief Identifies if the specified element of the collection has an error status
     * @param enumeration - Enumeration index from 0 to EntryCount to check
   	  * @param status - Status value to be checked.
     * @returns - true if the specified element has an error status, false otherwise.
     */
    bool IsStatus(EnumerationType enumeration, StatusType status) const noexcept
    {
        uint8_t value = collection.get(std::to_underlying(enumeration));
        return (value == std::to_underlying( status ));
    }

    /**
     * @brief Identifies if the specified element of the collection is not the status
     * @param enumeration - Enumeration index from 0 to EntryCount to check
   	  * @param status - Status value to be checked.
     * @returns - true if the specified element has an error status, false otherwise.
     */
    bool IsNotStatus(EnumerationType enumeration, StatusType status) const noexcept
    {
        return !IsStatus(enumeration, status);
    }

	const char* ToStatusString(StatusType status) noexcept
	{

		if( statusToString)
		{
			return statusToString( status );
		}
		else
		{
			return "Unknown";
		}
	}

	const char* ToEnumerationString( EnumerationType enumeration) noexcept
	{
		if( enumToString)
		{
			return enumToString( enumeration );
		}
		else
		{
			return "Unknown";
		}
	}

private:

	const char* (*enumToString)( EnumerationType );
	const char* (*statusToString)( StatusType );
    MultibitSet<BitsPerStatus, EntryCount> collection;     ///< Bitset to store errors ( 2 bit status per entry ), using 0: unknown, 1:cleared, 2: set, 3:ignored

};


#endif // EnumeratedErrorSet
