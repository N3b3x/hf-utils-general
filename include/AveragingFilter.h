/**
 * @file AveragingFilter.h
 * @brief Templated moving average filter.
 *
 * Nebula Tech Corporation
 *
 * Copyright © 2023 Nebula Tech Corporation.   All Rights Reserved.
 * This file is part of HardFOC and is licensed under the GNU General Public
 * License v3.0 or later.
 *
 * Contains the declaration of the AveragingFilter class that provides an
 * averaging or windowed filter.
 */
#ifndef AveragingFilter_h
#define AveragingFilter_h

#include <array>
#include <cstdint>

template <typename DataType, uint32_t WindowSize> class AveragingFilter
{
    public:

      /// <summary>
      /// Constructor allocates an instance of the class.   
      /// </summary>

      AveragingFilter() noexcept;

      /// <summary>
      /// Copy constructor and assignment operator are disabled. 
      /// </summary>
    
      AveragingFilter(const AveragingFilter&) = delete;

      AveragingFilter& operator =(const AveragingFilter&) = delete;

      /// <summary>
      /// Destructor is simple default, since there are no allocations
      /// </summary>
      
      ~AveragingFilter() = default;

      /// <summary>
      /// This function adds a value to the filter.   
      /// </summary>
      /// <param name="value"> Value to add to the buffer</param>
      /// <returns>true if filter is "full", false otherwise</returns>

      bool Append(DataType value ) noexcept;

      /// <summary>
      /// This function adds a value to the filter, but does not track whether it is full.     
      /// </summary>
      /// <param name="value"> Value to add to the buffer</param>

      void FastAppend(DataType value ) noexcept;

      /// <summary>
      /// This function is used to clear the filter.     
      /// </summary>
      //<returns>Returns the current average value</returns>

      DataType Reset() noexcept;

      /// <summary>
      /// This function returns the number of values in the filter.
      /// </summary>
      ///<returns>The number of  values in the filter.</returns>

      uint32_t GetCount() const noexcept;

      /// <summary>
      /// This function determines whether the filter is fully loaded with values.
      /// </summary>
      ///<returns>true if the filter is fully loaded, false otherwise</returns>

      bool IsFullyLoaded() const noexcept;
	
      /// <summary>
      /// This function determines whether the filter values are all within the range
      /// </summary>
      ///<returns>true if the values in the filter are stable</returns>

      bool IsStable(DataType range ) noexcept;

      /// <summary>
      /// This function returns the current filtered value.
      /// </summary>
      /// <returns>Filtered value</returns>

      float GetValue() const noexcept;
      
      /// <summary>
      /// This function identifies the min and max values in the filter, along with their
      ///   offsets.
      /// </summary>
      /// <param name="minValue">Minimum value in filter</param>
      /// <param name="minOffset">Offset to minimum value in filter</param>
      /// <param name="maxValue">Maximum value in filter</param>
      /// <param name="maxOffset">Offset to maximum value in filter</param>
      /// <returns>Number of values in filter</returns>

      uint32_t GetMinMax(DataType& minValue, std::size_t& minOffset, DataType& maxValue, std::size_t& maxOffset) const noexcept;
      
      /// <summary>
      /// This function returns the number of elements in the filter.
      /// </summary>
      /// <returns>Number of elements in filter</returns>

      std::size_t size() const noexcept;
       
   private:

      uint32_t   valuesLoadedCount;     // Number of values currently in filter
      uint32_t   nextSlot;              // Next location to insert a value
      std::array<DataType, WindowSize> filter;
};


template <uint32_t WindowSize> class AveragingFilter<uint16_t, WindowSize>
{
   public:
      /// <summary>
      /// Constructor allocates an instance of the class. 
      /// </summary>

      AveragingFilter() noexcept;

      /// <summary>
      /// Copy constructor and assignment operator are disabled. 
      /// </summary>

      AveragingFilter(const AveragingFilter&) = delete;

      AveragingFilter& operator =(const AveragingFilter&) = delete;
       
      /// <summary>
      /// Destructor is simple default, since there are no allocations
      /// </summary>

      ~AveragingFilter() = default;

      /// <summary>
      /// This function adds a value to the filter.   
      /// </summary>
      /// <param name="value"> Value to add to the buffer</param>
      /// <returns>true if filter is "full", false otherwise</returns>


      bool Append(uint16_t value ) noexcept;
   
      /// <summary>
      /// This function adds a value to the filter, but does not track whether it is full.     
      /// </summary>
      /// <param name="value"> Value to add to the buffer</param>

      void FastAppend( uint16_t value ) noexcept;

      /// <summary>
      /// This function is used to clear the filter.     
      /// </summary>
      //<returns>Returns the current average value</returns>

      uint16_t Reset() noexcept;

      /// <summary>
      /// This function returns the number of values in the filter.
      /// </summary>
      ///<returns>The number of  values in the filter.</returns>

      uint32_t GetCount() const noexcept;

      /// <summary>
      /// This function determines whether the filter is fully loaded with values.
      /// </summary>
      ///<returns>true if the filter is fully loaded, false otherwise</returns>

       bool IsFullyLoaded() const noexcept;

      /// <summary>
      /// This function returns the current filtered value.
      /// </summary>
      /// <returns>Filtered value</returns>

      float GetValue() const noexcept;
      
      /// <summary>
      /// This function identifies the min and max values in the filter, along with their
      ///   offsets.
      /// </summary>
      /// <param name="minValue">Minimum value in filter</param>
      /// <param name="minOffset">Offset to minimum value in filter</param>
      /// <param name="maxValue">Maximum value in filter</param>
      /// <param name="maxOffset">Offset to maximum value in filter</param>
      /// <returns>Number of values in filter</returns>

      uint32_t GetMinMax( uint16_t& minValue, std::size_t& minOffset, uint16_t& maxValue, std::size_t& maxOffset ) const noexcept;

      std::size_t size() const noexcept;
       
    private:

      uint32_t   valuesLoadedCount;     // Number of values currently in filter
      uint32_t   nextSlot;              // Next location to insert a value
      std::array<uint16_t, WindowSize> filter;
};

/// <summary>
/// Constructor allocates an instance of the class.   
/// </summary>

template <typename DataType, uint32_t WindowSize> AveragingFilter<DataType, WindowSize>::AveragingFilter() noexcept :
   valuesLoadedCount(0),
   nextSlot(0),
   filter()
{
    // No code at this time
}

/// <summary>
/// This function adds a value to the filter.   
/// </summary>
/// <param name="value"> Value to add to the buffer</param>
/// <returns>true if filter is "full", false otherwise</returns>

template < typename DataType, uint32_t WindowSize> bool AveragingFilter<DataType, WindowSize>::Append(DataType value) noexcept
{
    if( nextSlot < WindowSize )  // e.g.  for 4 slot window, next slot 3 is max
    {
        filter[nextSlot++] = value;
        if( valuesLoadedCount < WindowSize)  // Only count up until full
        {
            valuesLoadedCount++;
        }
    }
    else  // Wrap-around, stop counting values
    {
        filter[0] = value;
        nextSlot = 1;
    }
    return valuesLoadedCount == WindowSize;
}

/// <summary>
/// This function adds a value to the filter, but does not track whether it is full.     
/// </summary>
/// <param name="value"> Value to add to the buffer</param>

template < typename DataType, uint32_t WindowSize> void AveragingFilter<DataType, WindowSize>::FastAppend(DataType value) noexcept
{
    if( nextSlot < WindowSize )  // e.g.  for 4 slot window, next slot 3 is max
    {
        filter[nextSlot++] = value;
        if( valuesLoadedCount < WindowSize)  // Only count up until full
        {
            valuesLoadedCount++;
        }
    }
    else  // Wrap-around, stop counting values
    {
        filter[0] = value;
        nextSlot = 1;
    }
}

/// <summary>
/// This function is used to clear the filter.     
/// </summary>
///<returns>Returns the current average value</returns>

template <typename DataType, uint32_t WindowSize> DataType AveragingFilter<DataType, WindowSize>::Reset() noexcept
{
   float value = GetValue();
   valuesLoadedCount = 0;
   nextSlot = 0;
   return (DataType)value;
}

/// <summary>
/// This function returns the number of values in the filter.
/// </summary>
///<returns>The number of  values in the filter.</returns>

template <typename DataType, uint32_t WindowSize> uint32_t AveragingFilter<DataType, WindowSize>::GetCount() const noexcept
{
    return( valuesLoadedCount );
}

// This function determines whether the filter is fully loaded with values.
// Arguments:  n/a
// Returns:    true if the filter is fully loaded, false otherwise

template <typename DataType, uint32_t WindowSize> bool AveragingFilter<DataType, WindowSize>::IsFullyLoaded() const noexcept
{
    return ( valuesLoadedCount == WindowSize );
}

/// <summary>
/// This function returns the current filtered value.
/// </summary>
/// <returns>Filtered value</returns>

template <typename DataType, uint32_t WindowSize> float AveragingFilter<DataType, WindowSize>::GetValue() const noexcept
{
    if ( valuesLoadedCount > 0  )
    {
        double sum = static_cast<double>(filter[0]);
        for( uint32_t filterIndex = 1; filterIndex < valuesLoadedCount; ++filterIndex)
        {
            sum += double(filter[filterIndex]);
        }
        return float( sum / valuesLoadedCount );
    }
    return 0.0F;
}
/// <summary>
/// This function identifies the min and max values in the filter, along with their
///   offsets.
/// </summary>
/// <param name="minValue">Minimum value in filter</param>
/// <param name="minOffset">Offset to minimum value in filter</param>
/// <param name="maxValue">Maximum value in filter</param>
/// <param name="maxOffset">Offset to maximum value in filter</param>
/// <returns>Number of values in filter</returns>

template <typename DataType, uint32_t WindowSize> uint32_t AveragingFilter<DataType, WindowSize>::GetMinMax( 
       DataType& minValue, std::size_t& minOffset, DataType& maxValue, std::size_t& maxOffset ) const noexcept
{
   if ( valuesLoadedCount > 0  )
   {
     // Presume the first value is both min and max.  Then look for something more extreme.
      
      minValue = maxValue = filter[0]; 
      minOffset = maxOffset = 0;
      
     for( uint32_t filterIndex = 1; filterIndex < valuesLoadedCount; ++filterIndex)
     {
         if( minValue > filter[filterIndex] )
         {
            minValue = filter[filterIndex];
            minOffset = filterIndex;
         }
         else if( maxValue < filter[filterIndex] )
         {
            maxValue = filter[filterIndex];
            maxOffset = filterIndex;
         }
     }
   }
   return valuesLoadedCount;
}

/// <summary>
/// This function returns the number of elements in the filter.
/// </summary>
/// <returns>Number of elements in filter</returns>

template <typename DataType, uint32_t WindowSize> std::size_t AveragingFilter<DataType, WindowSize>::size() const noexcept
{
    return filter.size();
}

/// <summary>
/// Copy constructor and assignment operator use default deep copy.   
/// </summary>

template <uint32_t WindowSize> AveragingFilter<uint16_t, WindowSize>::AveragingFilter() noexcept :
   valuesLoadedCount(0),
   nextSlot(0),
   filter()
{
    // No code at this time
}

/// <summary>
/// This function adds a value to the filter.   
/// </summary>
/// <param name="value"> Value to add to the buffer</param>
/// <returns>true if filter is "full", false otherwise</returns>

template <uint32_t WindowSize> bool AveragingFilter<uint16_t, WindowSize>::Append(uint16_t value) noexcept
{
    if( nextSlot < WindowSize )  // e.g.  for 4 slot window, next slot 3 is max
    {
        filter[nextSlot++] = value;
        if( valuesLoadedCount < WindowSize)  // Only count up until full
        {
            valuesLoadedCount++;
        }
    }
    else  // Wrap-around, stop counting values
    {
        filter[0] = value;
        nextSlot = 1;
    }
    return valuesLoadedCount == WindowSize;
}

/// <summary>
/// This function adds a value to the filter, but does not track whether it is full.     
/// </summary>
/// <param name="value"> Value to add to the buffer</param>

template <uint32_t WindowSize> void AveragingFilter<uint16_t, WindowSize>::FastAppend(uint16_t value) noexcept
{
    if( nextSlot < WindowSize )  // e.g.  for 4 slot window, next slot 3 is max
    {
        filter[nextSlot++] = value;
        if( valuesLoadedCount < WindowSize)  // Only count up until full
        {
            ++valuesLoadedCount;
        }
    }
    else  // Wrap-around, stop counting values
    {
        filter[0] = value;
        nextSlot = 1;
    }
}

/// <summary>
/// This function is used to clear the filter.     
/// </summary>
///<returns>Returns the current average value</returns>

template <uint32_t WindowSize> uint16_t AveragingFilter<uint16_t, WindowSize>::Reset() noexcept
{
   if ( valuesLoadedCount > 0  )
   {
        uint32_t sum = filter[0];
        for( uint32_t filterIndex = 1; filterIndex < valuesLoadedCount; ++ filterIndex )
        {
           sum += filter[filterIndex];
        }
        valuesLoadedCount = 0;
        nextSlot = 0;
        return static_cast<uint16_t>( sum  / valuesLoadedCount);
   }
   return 0;
}

///<summary>
/// This function returns the number of values in the filter.
///</summary>
///<returns>The number of  values in the filter.</returns>

template <uint32_t WindowSize> uint32_t AveragingFilter<uint16_t, WindowSize>::GetCount() const noexcept
{
    return( valuesLoadedCount );
}

// This function determines whether the filter is fully loaded with values.
// Arguments:  n/a
// Returns:    true if the filter is fully loaded, false otherwise

template <uint32_t WindowSize> bool AveragingFilter<uint16_t, WindowSize>::IsFullyLoaded() const noexcept
{
    return ( valuesLoadedCount == WindowSize );
}

template < typename DataType, uint32_t WindowSize> bool AveragingFilter<DataType, WindowSize>::IsStable(DataType range) noexcept
{
	uint32_t max = filter[0];
	uint32_t min = filter[0];
	for (uint32_t i = 0; i < WindowSize; i++)
	{
		if (filter[i] < min)
		{
			min = filter[i];	
		}
		if (filter[i] > max)
		{
			max = filter[i];
		}
	}
	
	if ((max - min) <= range)
	{
		return true;	
	}
	else
	{
		return false;	
	}
}

/// <summary>
/// This function determines whether the filter is fully loaded with values.
/// </summary>
///<returns>true if the filter is fully loaded, false otherwise</returns>

template <uint32_t WindowSize> float AveragingFilter<uint16_t, WindowSize>::GetValue() const noexcept
{
    if ( valuesLoadedCount > 0  )
    {
        uint32_t sum = filter[0];
        for( uint32_t filterIndex = 1; filterIndex < valuesLoadedCount; ++ filterIndex )
        {
           sum += filter[filterIndex];
        }
        return float( sum ) / float( valuesLoadedCount );
    }
    return 0;
}
/// <summary>
/// This function identifies the min and max values in the filter, along with their
///   offsets.
/// </summary>
/// <param name="minValue">Minimum value in filter</param>
/// <param name="minOffset">Offset to minimum value in filter</param>
/// <param name="maxValue">Maximum value in filter</param>
/// <param name="maxOffset">Offset to maximum value in filter</param>
/// <returns>Number of values in filter</returns>

template <uint32_t WindowSize> uint32_t AveragingFilter<uint16_t, WindowSize>::GetMinMax( 
       uint16_t& minValue, std::size_t& minOffset, uint16_t& maxValue, std::size_t& maxOffset ) const noexcept
{
   if ( valuesLoadedCount > 0  )
   {
     // Presume the first value is both min and max.  Then look for something more extreme.
      
      minValue = maxValue = filter[0]; 
      minOffset = maxOffset = 0;
      
     for( uint32_t filterIndex = 1; filterIndex < valuesLoadedCount; ++filterIndex)
     {
         if( minValue > filter[filterIndex] )
         {
            minValue = filter[filterIndex];
            minOffset = filterIndex;
         }
         else if( maxValue < filter[filterIndex] )
         {
            maxValue = filter[filterIndex];
            maxOffset = filterIndex;
         }
     }
   }
   return valuesLoadedCount;
}
/// <summary>
/// This function returns the number of elements in the filter.
/// </summary>
/// <returns>Number of elements in filter</returns>

template <uint32_t WindowSize> std::size_t AveragingFilter<uint16_t, WindowSize>::size() const noexcept
{
    return filter.size();
}

#endif  // AveragingFilter_h
