/**
 * @file Utility.h
 * @brief Collection of generic helper functions.
 *
 * Nebula Tech Corporation
 *
 * Copyright © 2023 Nebula Tech Corporation.   All Rights Reserved.
 * This file is part of HardFOC and is licensed under the GNU General Public
 * License v3.0 or later.
 *
 * Contains the declaration and definition of general-purpose utility functions.
 * Note: These functions are not thread or interrupt-safe and should be called
 * with appropriate guards if used within an ISR or shared between tasks.
 */

#ifndef UTILITY_H_
#define UTILITY_H_

#include <cstdint>
#include <algorithm>
#include <functional>
#include <utility>
#include <stdbool.h>
#include <atomic>

#ifdef __cplusplus
#include <string>
#include <vector>
#endif
#include "math.h"

#ifdef __cplusplus
/**
 * @brief Converts a string to lowercase.
 *
 * This function takes a string and converts all its characters to lowercase.
 *
 * @param str The input string to be converted.
 * @return A new string with all characters in lowercase.
 */
std::string StringToLower(const std::string& str);

/**
 * @brief Converts a string to uppercase.
 *
 * This function takes a string and converts all its characters to uppercase.
 *
 * @param str The input string to be converted.
 * @return A new string with all characters in uppercase.
 */
std::string StringToUpper(const std::string& str);

/**
 * @brief Trims whitespace from both ends of a string.
 *
 * This function removes leading and trailing whitespace characters from the input string.
 *
 * @param str The input string to be trimmed.
 * @return A new string with leading and trailing whitespace removed.
 */
std::string StringTrim(const std::string& str);

/**
 * @brief Splits a string into a vector of strings using a specified delimiter.
 *
 * This function takes a string and splits it into a vector of substrings based on the specified delimiter.
 *
 * @param str The input string to be split.
 * @param delimiter The character used to split the string.
 * @return A vector of substrings.
 */
std::vector<std::string> StringSplit(const std::string& str, char delimiter);
#endif

#ifndef CONSOLEPORTWRITE_H
#define CONSOLEPORTWRITE_H
#define CONSOLE_WRITE(c,s) if((c)) { consolePort.Write((s)); }
#define CONSOLE_PWRITE(c,s) if((c)) { p_consolePort->Write((s)); }
#endif // CONSOLEPORTWRITE_H

#ifndef TIMECONVERSION_H
    #define TIMECONVERSION_H

    /**
     * @brief Converts a given time value from one unit to another.
     *
     * This function receives an input value in a specific time unit and converts it
     * to the desired output time unit. The conversion is done based on standard time
     * conversion ratios, considering the typical time unit conversions (e.g., 1 year is
     * approximately 31536000 seconds).
     *
     * @param inputValue The time value to be converted.
     * @param inputUnit The unit of the inputValue.
     * @param[out] outputValue The converted time value in the desired unit. The result is stored here.
     * @param outputUnit The desired output unit for the conversion.
     *
     * @return Returns true if the conversion is successful. If any of the input or output units
     * is not recognized, it returns false.
     *
     * @note The conversion for years does not account for leap years, and it's an approximation.
     */
    bool ConvertTime(float inputValue, TimeUnit inputUnit, float &outputValue, TimeUnit outputUnit);

#endif /* TIME_CONVERSION_H */

#ifndef PRESSURE_CONVERSION_H
    #define PRESSURE_CONVERSION_H

    /**
     * @brief Converts pressure from one unit to another.
     *
     * @tparam fromUnit The unit of the input pressure.
     * @tparam toUnit The unit of the output pressure.
     * @param pressure The pressure value to convert.
     * @return The converted pressure value.
     */
    template<pressure_unit_id_t fromUnit, pressure_unit_id_t toUnit>
    constexpr float ConvertPressureUnit(float pressure);

	/**
	 * @brief Convert pressure from one unit to another
	 *
	 * @param pressure Pressure to convert
	 * @param fromUnit Unit ID of the input pressure
     * @param[out] outputValue The converted value in the desired unit. The result is stored here.
	 * @param toUnit Unit ID to convert the pressure to
	 *
	 * @return Returns true if the conversion is successful. If any of the input or output units
     * is not recognized, it returns false.
	 */
    bool ConvertPressureUnit(float pressure, pressure_unit_id_t fromUnit, float &outputValue, pressure_unit_id_t toUnit);

#endif /* PRESSURE_CONVERSION_H */

#ifndef FLOW_CONVERSION_H
    #define FLOW_CONVERSION_H

    /**
     * @brief Converts flow from one unit to another.
     *
     * @tparam fromUnit The unit of the input flow.
     * @tparam toUnit The unit of the output flow.
     * @param flow The flow value to convert.
     * @return The converted flow value.
     */
    template<flow_unit_id_t fromUnit, flow_unit_id_t toUnit>
    constexpr float ConvertFlowUnit(float flow);

    /**
     * @brief Convert flow from one unit to another
     *
     * @param flow Flow to convert
     * @param fromUnit Unit ID of the input flow
     * @param[out] outputValue The converted value in the desired unit. The result is stored here.
     * @param toUnit Unit ID to convert the flow to
     *
     * @return Returns true if the conversion is successful. If any of the input or output units
     * is not recognized, it returns false.
     */
    bool ConvertFlowUnit(float flow, flow_unit_id_t fromUnit, float &outputValue, flow_unit_id_t toUnit);

#endif /* FLOW_CONVERSION_H */

#ifndef TEMPERATURE_CONVERSION_H
    #define TEMPERATURE_CONVERSION_H

    /**
     * @brief Converts temperature from one unit to another.
     *
     * @tparam fromUnit The unit of the input temperature.
     * @tparam toUnit The unit of the output temperature.
     * @param temp The temperature value to convert.
     * @return The converted temperature value.
     */
    template<temp_unit_id_t fromUnit, temp_unit_id_t toUnit>
    constexpr float ConvertTemperatureUnit(float temp);

	/**
	 * @brief Convert pressure from one unit to another
	 *
	 * @param pressure Pressure to convert
	 * @param fromUnit Unit ID of the input pressure
     * @param[out] outputValue The converted value in the desired unit. The result is stored here.
	 * @param toUnit Unit ID to convert the pressure to
	 *
	 * @return Returns true if the conversion is successful. If any of the input or output units
     * is not recognized, it returns false.
	 */
    bool ConvertTemperatureUnit(float temp, temp_unit_id_t fromUnit, float &outputValue, temp_unit_id_t toUnit);

#endif /* TEMPERATURE_CONVERSION_H */


#ifndef LOGICTESTER_H
    #define LOGICTESTER_H

    /**
     * @brief Test a logic repeatedly until it produces the expected result or a timeout is reached.
     *
     * @param logic The logic to test, should return a bool.
     * @param expected The expected result, true or false.
     * @param timeoutMs The timeout in milliseconds.
     * @param timeBetweenChecksMs Time waited between every check of the logic, will be bound to timeoutMs (default 1)
     * @param pTimeTakenSaver pointer to data saver if want the time elapsed before returning result.
     *
     * @return bool True if logic produced the expected result within the timeout, false otherwise.
     */
    bool TestLogicWithTimeout(const std::function<bool()>& logic, bool expected, uint32_t timeoutMs, uint32_t timeBetweenChecksMs, uint32_t* pTimeTakenSaver=nullptr);
#endif /* LOGICTESTER_H */

#ifndef HIGH
/**
 * @brief Logic level high value.
 *
 * Defined as 1 to avoid dependency on platform specific
 * definitions such as IOPORT_LEVEL_HIGH.
 */
#define HIGH 1
#endif

#ifndef LOW
/**
 * @brief Logic level low value.
 *
 * Defined as 0 to keep the utility layer hardware agnostic.
 */
#define LOW 0
#endif


#ifndef UNUSED
	#define UNUSED(x) (void)(x)
#endif

#ifndef MIN
/**
 * @brief Returns the smaller of two values.
 */
#define MIN(a,b) (((a)<(b)) ? (a) : (b))
#endif

#ifndef MAX
/**
 * @brief Returns the larger of two values.
 */
#define MAX(a,b) (((a)>(b)) ? (a) : (b))
#endif
#ifndef MAP
/**
 * @brief Linearly maps a value from one range to another.
 */
#ifdef __cplusplus
template <typename T, typename InMin, typename InMax, typename OutMin, typename OutMax>
T MAP(T in_val, InMin in_min, InMax in_max, OutMin out_min, OutMax out_max) {
    return static_cast<T>((in_val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
}
#else
    #define MAP(in_val, in_min, in_max, out_min, out_max) \
        ((in_val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min)
#endif /* __cplusplus */
#endif /* MAP */

#ifndef CLAMP
/**
 * @brief Clamps a value between a minimum and maximum.
 */
#ifdef __cplusplus
template <typename T, typename MinVal, typename MaxVal>
T Clamp(T val, MinVal min_val, MaxVal max_val) {
    return std::clamp(val, min_val, max_val);
}
#else
#define CLAMP(val, min_val, max_val) \
    ((val) < (min_val) ? (min_val) : ((val) > (max_val) ? (max_val) : (val)))
#endif /* __cplusplus */
#endif /* CLAMP */

#ifndef MAP_WITH_BOUND
/**
 * @brief Maps a value and constrains the output within bounds.
 */
#ifdef __cplusplus
template <typename T, typename InMin, typename InMax, typename OutMin, typename OutMax>
T MAP_WITH_BOUND(T in_val, InMin in_min, InMax in_max, OutMin out_min, OutMax out_max) {
    return Clamp(static_cast<T>((in_val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min), static_cast<T>(out_min), static_cast<T>(out_max));
}
#else
    #define MAP_WITH_BOUND(in_val, in_min, in_max, out_min, out_max) \
        (CLAMP(((in_val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min), out_min, out_max))
#endif /* __cplusplus */
#endif /* MAP_WITH_BOUND */
#ifndef CONSTRAIN
/**
 * @brief Constrains a value within a given range.
 */
#ifdef __cplusplus
	template<typename T, typename Low, typename High>
	T CONSTRAIN(T val, Low low, High high) {
		return val < static_cast<T>(low) ? static_cast<T>(low) : (val > static_cast<T>(high) ? static_cast<T>(high) : val);
	}
#else
	#define CONSTRAIN(val,low,high) ((val)<(low)?(low):((val)>(high)?(high):(val)))
#endif /* __cplusplus */
#endif /* CONSTRAIN */
#ifndef IS_IN_BOUND
/**
 * @brief Checks if a value lies within a range.
 */
#ifdef __cplusplus
    template<typename T, typename Low, typename High>
    bool IS_IN_BOUND(T val, Low low, High high) {
        return val >= static_cast<T>(low) && val <= static_cast<T>(high);
    }

    // For setpoint and threshold
    template<typename T, typename Setpoint, typename Threshold>
    bool IS_IN_BOUND_THRESHOLD(T val, Setpoint setpoint, Threshold threshold) {
        T low = static_cast<T>(setpoint) - static_cast<T>(threshold);
        T high = static_cast<T>(setpoint) + static_cast<T>(threshold);
        return val >= low && val <= high;
    }
#else
    #define IS_IN_BOUND(val,low,high) ((val)>=(low) && (val)<=(high))
    #define IS_IN_BOUND_THRESHOLD(val, setpoint, threshold) ((val) >= ((setpoint) - (threshold)) && (val) <= ((setpoint) + (threshold)))
#endif /* __cplusplus */
#endif /* IS_IN_BOUND */

#ifndef IS_NOT_IN_BOUND
/**
 * @brief Checks if a value lies outside a range.
 */
#ifdef __cplusplus
    template<typename T, typename Low, typename High>
    bool IS_NOT_IN_BOUND(T val, Low low, High high) {
        return !IS_IN_BOUND(val, low, high);
    }

    // For setpoint and threshold
    template<typename T, typename Setpoint, typename Threshold>
    bool IS_NOT_IN_BOUND_THRESHOLD(T val, Setpoint setpoint, Threshold threshold) {
        return !IS_IN_BOUND_THRESHOLD(val, setpoint, threshold);
    }
#else
    #define IS_NOT_IN_BOUND(val,low,high) (!IS_IN_BOUND(val,low,high))
#endif /* __cplusplus */
#endif /* IS_NOT_IN_BOUND */


#ifndef LINEAR_INTERPOLATE
/**
 * @brief Performs a simple linear interpolation.
 */
#ifdef __cplusplus
    template<typename T>
    T LINEAR_INTERPOLATE(T x_val, T x1, T y1, T x2, T y2) {
        return y1 + (y2 - y1) * ((x_val - x1) / (x2 - x1));
    }
#else
    #define LINEAR_INTERPOLATE(x_val,x1,y1,x2,y2) (y1 + (y2 - y1) * ((x_val - x1) / (x2 - x1)))
#endif /* __cplusplus */
#endif /* LINEAR_INTERPOLATE */


// Checks if a bit set in a byte
#ifdef __cplusplus
template<typename T, typename Pos>
	T IS_BIT_SET(T byte, Pos pos) {
		return static_cast<T>((byte >> static_cast<T>(pos)) & static_cast<T>(1u));
	}
#else
	#define IS_BIT_SET(byte, pos) 	((1 & (byte >> pos)))
#endif

#ifdef __cplusplus
template<typename T>
bool ARE_BITS_SET(T byte, T mask) {
    return (byte & mask) == mask;
}
#else
#define ARE_BITS_SET(byte, mask) ((byte & mask) == mask)
#endif

// Clear Bit
#ifdef __cplusplus
	template<typename T, typename Pos>
	T ClearBit(T byte, Pos pos) {
		return static_cast<T>(byte & ~(static_cast<T>(1u) << static_cast<T>(pos)));
	}
#else
	#define CLEAR_BIT(byte, pos)  (byte & ~(static_cast<uint16_t>(1) << pos))
#endif

#ifndef ARRAY_SIZE
#ifdef __cplusplus
    template<typename T, std::size_t N>
    constexpr std::size_t ARRAY_SIZE(T (&)[N]) {
        return N;
    }
#else
    #define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#endif /* __cplusplus */
#endif /* ARRAY_SIZE */

#ifndef GET_NEXT_SNAP_POINT_DIVISIONS
#ifdef __cplusplus
    template<typename T>
    constexpr T GetNextSnapPointDivision(T num, T lower, T upper, std::size_t divisions) {
        if (divisions == 0) {
            return lower; // Return lower if number of divisions is zero
        }
        if (num < lower) { return lower; }
        if (num > upper) { return upper; }

        T range = upper - lower;
        T divisionSize = range / divisions;
        return ((num - lower) / divisionSize + 1) * divisionSize + lower;
    }
#else
    #define GET_NEXT_SNAP_POINT_DIVISIONS(num, lower, upper, divisions) ((divisions == 0 || num < lower || num > upper) ? lower : ((num - lower) / divisions + 1) * divisions + lower)
#endif /* __cplusplus */
#endif /* GET_NEXT_SNAP_POINT_DIVISIONS */

#ifndef GET_NEXT_SNAP_POINT_DELTA
#ifdef __cplusplus
    template<typename T>
    T GetNextSnapPointDelta(T num, T lower, T upper, T delta) {
        if (delta <= 0) {
            return lower; // Return lower if delta is less than or equal to zero
        }
        if (num < lower) { return lower; }
        if (num > upper) { return upper; }

        return ((num - lower) / delta + 1) * delta + lower;
    }
#else
    #define GET_NEXT_SNAP_POINT_DELTA(num, lower, upper, delta) ((delta <= 0 || num < lower || num > upper) ? lower : ((num - lower) / delta + 1) * delta + lower)
#endif /* __cplusplus */
#endif /* GET_NEXT_SNAP_POINT_DELTA */

#ifndef GET_SNAP_POINT_IN_WINDOW
#ifdef __cplusplus
template<typename T>
T GetSnapPointInWindow(T num, T windowMin, T windowMax, T delta) {
    if (delta <= 0) {
        return windowMin; // Return windowMin if delta is less than or equal to zero
    }
    if (num < windowMin) {
        return windowMin;
    }

    if (num > windowMax) {
        return windowMax;
    }

    // Calculate the snappable point within the window
    T numRelativeToWindow = num - windowMin;
    T numDeltasFromZero = numRelativeToWindow / delta;
    T snappedValue = std::floor(numDeltasFromZero) * delta + windowMin;

    // Ensure the snapped value does not exceed the windowMax
    return std::min(snappedValue, windowMax);
}
#else
#define GET_SNAP_POINT_IN_WINDOW(num, max, windowMin, windowMax, delta) \
    ((delta <= 0 || num < windowMin || num > windowMax) ? windowMin : \
    (((num - windowMin) / delta) * delta + windowMin > windowMax) ? windowMax : \
    ((num - windowMin) / delta) * delta + windowMin)
#endif /* __cplusplus */
#endif /* GET_SNAP_POINT_IN_WINDOW */



#ifndef GET_SNAP_POINT
#ifdef __cplusplus
    template<typename T>
    bool GetSnapPoint(std::size_t section_number, std::size_t total_divisions, T total_length, T& snap_point) {
        if (section_number > total_divisions) {
            snap_point = total_length;
            return false;
        } else {
            snap_point = (total_length / total_divisions) * section_number;
            return true;
        }
    }

    template<typename T>
    bool GetSnapPoint(size_t section_number, size_t total_divisions, T start_point, T end_point, T& snap_point) {
        if (section_number > total_divisions) {
            snap_point = end_point;
            return false;
        } else {
            double total_length = end_point - start_point;
            snap_point = (total_length / total_divisions) * section_number + start_point;
            return true;
        }
    }

#else
    bool GetSnapPoint(size_t section_number, size_t total_divisions, float total_length, float* snap_point) {
        if (section_number > total_divisions) {
            *snap_point = total_length;
            return false;
        } else {
            *snap_point = (total_length / total_divisions) * section_number;
            return true;
        }
    }

    bool GetSnapPointWithBounds(size_t section_number, size_t total_divisions, float start_point, float end_point, float* snap_point) {
        if (section_number > total_divisions) {
            *snap_point = end_point;
            return false;
        } else {
            double total_length = end_point - start_point;
            *snap_point = (total_length / total_divisions) * section_number + start_point;
            return true;
        }
    }

#endif /* __cplusplus */
#endif /* GET_SNAP_POINT */


#ifndef CAST_Sn_TO_S32
#ifdef __cplusplus
/**
     * @brief Casts an n-bit signed integer to a 32-bit signed integer.
     *
     * This function checks the MSB of the signed integer (Bit n).
     * If it is 1, indicating the value is negative, Bits 32 to n+1 are set to 1.
     * If it is 0, the value remains unchanged.
     *
     * @tparam T Data type of the value. Typically an integer type.
     * @tparam nBit The bit width of the integer.
     * @param value The n-bit signed integer value.
     * @return T The value casted to 32-bit.
     */
    template <typename T, size_t nBit>
    T CAST_Sn_TO_S32(T value) {
        static_assert(std::is_integral<T>::value, "T should be an integral type");
        return value | ((value & (1<<(nBit-1))) ? ~((0x1<<nBit)-1) : 0);
    }
#else
    /**
     * @brief Macro to cast an n-bit signed integer to a 32-bit signed integer.
     *
     * This checks the MSB of the signed integer (Bit n).
     * If it is 1, indicating the value is negative, Bits 32 to n+1 are set to 1.
     * If it is 0, the value remains unchanged.
     */
    #define CAST_Sn_TO_S32(value, n)  ((value) | (((value) & (1<<((n)-1))) ? ~((0x1<<(n))-1) : 0))
#endif /* __cplusplus */
#endif /* CAST_Sn_TO_S32 */


// Generic mask/shift macros
#define FIELD_GET(data, mask, shift) \
	(((data) & (mask)) >> (shift))
#define FIELD_SET(data, mask, shift, value) \
	(((data) & (~(mask))) | (((value) << (shift)) & (mask)))

// Register read/write/update macros using Mask/Shift:
#define FIELD_READ(read, motor, address, mask, shift) \
	FIELD_GET(read(motor, address), mask, shift)
#define FIELD_WRITE(write, motor, address, mask, shift, value) \
	(write(motor, address, ((value)<<(shift)) & (mask)))
#define FIELD_UPDATE(read, write, motor, address, mask, shift, value) \
	(write(motor, address, FIELD_SET(read(motor, address), mask, shift, value)))

// Memory access helpers
// Force the compiler to access a location exactly once
#ifndef typeof
#ifdef __cplusplus
    #define typeof(x)  decltype(x)
#else
    #define typeof(x)  __typeof__(x)
#endif /* __cplusplus */
#endif /* typeof */

#ifdef __cplusplus
    template<typename T>
    T ACCESS_ONCE(T& x) {
        return std::atomic<T>{x}.load(std::memory_order_relaxed);
    }
#else
    #define ACCESS_ONCE(x) (*((volatile typeof(x) *) (&x)))
#endif /* __cplusplus */

// Macro to remove write bit for shadow register array access
#define TMC_ADDRESS(x) ((x) & (TMC_ADDRESS_MASK))


template <std::size_t N>
static constexpr float constexpr_sum(const std::array<float, N>& arr) {
	float sum = 0.0f;
	for (std::size_t i = 0; i < N; ++i) {
		sum += arr[i];
	}
	return sum;
}

// This is the implementation of the c++23 function;  provided if using an earlier compiler
#if (__cplusplus < 202101L)

namespace std
{
	template <typename Type> constexpr typename std::underlying_type<Type>::type to_underlying(Type e) noexcept
	{
	    return static_cast<typename std::underlying_type<Type>::type>(e);
	}
}

#endif
/**
 * @brief This function clips the first parameter to stay within the range of the minimum and maximum values.
 *
 * @tparam Type The type of the value to be limited.
 * @param[in,out] value Value to be limited.
 * @param[in] minValue Minimum allowed value.
 * @param[in] maxValue Maximum allowed value.
 */

template <typename Type> void Clip ( Type& value, const Type minValue, const Type maxValue ) noexcept
{
 //  static_assert ( minValue <= maxValue);
    if(  value <  std::min( minValue, maxValue ) )
    {
        value = minValue;
    }
    else if( value >  std::max( minValue, maxValue ) )
    {
        value = maxValue;
    }
}


/**
 * @brief This function checks if the first parameter is within the range of the values, independent or order
 * @tparam Type The type of the value to be checked.
 * @param[in,out] value Value to be limited.
 * @param[in] value1 Start of range
 * @param[in] value1 End of range.
 */
template <typename Type> bool InRange( Type& value, const Type& value1, const Type& value2 ) noexcept
{
    if( value1 <= value2 )
    {
    	return(value >=  value1 ) && ( value <= value2);
    }
    else
    {
    	return(value >=  value2 ) && ( value <= value1);
    }
}

// Helper functions to extract bytes from a four byte integer.

inline uint8_t Byte1( uint32_t value )
{
   return static_cast<uint8_t>((value >> 24 ) & 0x000000FF);
}
inline uint8_t Byte2( uint32_t value )
{
   return static_cast<uint8_t>((value >> 16 ) & 0x000000FF);
}
inline uint8_t Byte3( uint32_t value )
{
   return static_cast<uint8_t>((value >> 8 ) & 0x000000FF);
}
inline uint8_t Byte4( uint32_t value )
{
   return static_cast<uint8_t>((value ) & 0x000000FF);
}


inline void SetBits ( uint32_t& value, uint32_t bitMask )
{
    value |= bitMask;
}
inline void ClearBits ( uint32_t& value, uint32_t bitMask )
{
    value &= ~bitMask;
}

inline void SetBits ( uint8_t& value, uint8_t bitMask )
{
    value |= bitMask;
}

inline void ClearBits ( uint8_t& value, uint8_t bitMask )
{
    bitMask = ~bitMask;
    value &= bitMask;

}

static constexpr uint32_t FullBitMasks[] =
{
    0x00000001UL, 0x00000003UL, 0x00000007UL, 0x0000000FUL,
    0x0000001FUL, 0x0000003FUL, 0x0000007FUL, 0x000000FFUL,
    0x000001FFUL, 0x000003ffUL, 0x000007FFUL, 0x00000FFFUL,
    0x00001FFFUL, 0x00003FFFUL, 0x00007FFFUL, 0x0000FFFFUL,
    0x0001FFFFUL, 0x0003FFFFUL, 0x0007FFFFUL, 0x000FFFFFUL,
    0x001FFFFFUL, 0x003FFFFFUL, 0x007FFFFFUL, 0x00FFFFFFUL,
    0x01FFFFFFUL, 0x03FFFFFFUL, 0x07FFFFFFUL, 0x0FFFFFFFUL,
    0x1FFFFFFFUL, 0x3FFFFFFFUL, 0x7FFFFFFFUL, 0xFFFFFFFFUL
};

/*
 * This function creates a mask with the specified number of bits. It silently ignores values
 * greater than 32.
 */
constexpr uint32_t FullBitmask( const uint8_t bits ) noexcept
{
    return (bits > 0 ) ?  (( bits <= 32 ) ? FullBitMasks[( bits-1)] : 0xFFFFFFFF) : 0x00000000;
}

static constexpr uint32_t BitMask[] =
{
    1U << 0,   1U << 1U,  1U << 2U,  1U << 3U,
    1U << 4U,  1U << 5U,  1U << 6U,  1U << 7U,
    1U << 8U,  1U << 9U,  1U << 10U, 1U << 11,
    1U << 12U, 1U << 13U, 1U << 14U, 1U << 15U,
    1U << 16U, 1U << 17U, 1U << 18U, 1U << 19U,
    1U << 20U, 1U << 21U, 1U << 22U, 1U << 23U,
    1U << 24U, 1U << 25U, 1U << 26U, 1U << 27U,
    1U << 28U, 1U << 29U, 1U << 30U, 1U << 31U
};

constexpr uint32_t Bitmask ( uint8_t bit ) noexcept
{
    return (bit > 0 ) && ( bit <= 32 ) ? BitMask[bit-1] : 0x00000000;
}

/**
 * @brief This function converts a potentially signed value stored in an unsigned
 * variable, where the most significant bit (sign bit) is at the specified location.
 *
 * @param value Value containing a signed integer less than 32 bits.
 * @param msb Most significant bit (potential sign bit).
 * @return Corresponding signed value.
 */
int32_t TwosCompliment( uint32_t value, uint8_t msb ) noexcept;

/**
 * @brief This function takes a signed value which is less than 32 bits and converts it into
 * an unsigned value, ignoring the upper bits.
 *
 * @param value Value containing a signed integer less than 32 bits.
 * @param msb Most significant bit (potential sign bit).
 * @return Corresponding signed value.
 */
uint32_t TwosComplimentFrom( int32_t value, uint8_t msb ) noexcept;


/**
 * @brief Trivial function to get the break a uint16 into bytes and construct a uint16 from bytes.
 * @param value Value
 * @returns the byte of interest
 */
inline uint8_t Lsb( uint16_t value) noexcept
{
	return static_cast<uint8_t>(value & 0x00FF);
}

inline uint8_t Msb( uint16_t value )
{
	return static_cast<uint8_t>( ( value >> 8 ) & 0x00FF);
}

/**
 * @brief Trivial function to construct a uint16 from bytes.
 * @param msb Least significant byte
 * @param lsb Most significant byte
 * @returns the reconstructed value
 */
inline uint16_t ToUint16( uint8_t msb, uint8_t lsb ) noexcept
{
	return static_cast<uint16_t>(msb << 8u) + static_cast<uint16_t>(lsb);
}


template<pressure_unit_id_t fromUnit, pressure_unit_id_t toUnit>
constexpr float ConvertPressureUnit(float pressure)
{
    // Conversion constants
    constexpr float PA_TO_PA = 1.0f;
    constexpr float PA_TO_PSI = 0.000145038f;
    constexpr float PA_TO_BAR = 0.00001f;
    constexpr float PA_TO_ATM = 0.00000986923f;
    constexpr float PA_TO_MMHG = 0.00750062f;
    constexpr float PA_TO_INHG = 0.02953f;
    constexpr float PA_TO_MBAR = 0.01f;

    float inPascals = 0.0;

    // Convert from the fromUnit to Pascal
    if constexpr (fromUnit == pressure_unit_id_t::PRESSURE_UNIT_PSI) {
        inPascals = pressure / PA_TO_PSI;
    } else if constexpr (fromUnit == pressure_unit_id_t::PRESSURE_UNIT_PA) {
        inPascals = pressure * PA_TO_PA;
    } else if constexpr (fromUnit == pressure_unit_id_t::PRESSURE_UNIT_BAR) {
        inPascals = pressure / PA_TO_BAR;
    } else if constexpr (fromUnit == pressure_unit_id_t::PRESSURE_UNIT_ATM) {
        inPascals = pressure / PA_TO_ATM;
    } else if constexpr (fromUnit == pressure_unit_id_t::PRESSURE_UNIT_MMHG) {
        inPascals = pressure / PA_TO_MMHG;
    } else if constexpr (fromUnit == pressure_unit_id_t::PRESSURE_UNIT_INHG) {
        inPascals = pressure / PA_TO_INHG;
    } else if constexpr (fromUnit == pressure_unit_id_t::PRESSURE_UNIT_MBAR) {
        inPascals = pressure / PA_TO_MBAR;
    } else {
        return false;
    }

    // Convert from Pascal to the toUnit
    if constexpr (toUnit == pressure_unit_id_t::PRESSURE_UNIT_PSI) {
        return inPascals * PA_TO_PSI;
    } else if constexpr (toUnit == pressure_unit_id_t::PRESSURE_UNIT_PA) {
        return inPascals * PA_TO_PA;
    } else if constexpr (toUnit == pressure_unit_id_t::PRESSURE_UNIT_BAR) {
        return inPascals * PA_TO_BAR;
    } else if constexpr (toUnit == pressure_unit_id_t::PRESSURE_UNIT_ATM) {
        return inPascals * PA_TO_ATM;
    } else if constexpr (toUnit == pressure_unit_id_t::PRESSURE_UNIT_MMHG) {
        return inPascals * PA_TO_MMHG;
    } else if constexpr (toUnit == pressure_unit_id_t::PRESSURE_UNIT_INHG) {
        return inPascals * PA_TO_INHG;
    } else if constexpr (toUnit == pressure_unit_id_t::PRESSURE_UNIT_MBAR) {
        return inPascals * PA_TO_MBAR;
    } else {
        return false;
    }
}

template<flow_unit_id_t fromUnit, flow_unit_id_t toUnit>
constexpr float ConvertFlowUnit(float flow)
{
    // Conversion constants
    constexpr float SLPM_TO_SLPM = 1.0F;
    constexpr float SLPM_TO_CMH = 60.0F;
    constexpr float SLPM_TO_CFM = 0.0353147F;
    constexpr float SLPM_TO_CIS = 2.11888F;

    float inSLPM = 0.0;

    // Convert from the fromUnit to SLPM
    if constexpr (fromUnit == flow_unit_id_t::FLOW_UNIT_SLPM) {
        inSLPM = flow * SLPM_TO_SLPM;
    } else if constexpr (fromUnit == flow_unit_id_t::FLOW_UNIT_CMH) {
        inSLPM = flow / SLPM_TO_CMH;
    } else if constexpr (fromUnit == flow_unit_id_t::FLOW_UNIT_CFM) {
        inSLPM = flow / SLPM_TO_CFM;
    } else if constexpr (fromUnit == flow_unit_id_t::FLOW_UNIT_CIS) {
        inSLPM = flow / SLPM_TO_CIS;
    } else {
        return false;
    }

    // Convert from SLPM to the toUnit
    if constexpr (toUnit == flow_unit_id_t::FLOW_UNIT_SLPM) {
        return inSLPM * SLPM_TO_SLPM;
    } else if constexpr (toUnit == flow_unit_id_t::FLOW_UNIT_CMH) {
        return inSLPM * SLPM_TO_CMH;
    } else if constexpr (toUnit == flow_unit_id_t::FLOW_UNIT_CFM) {
        return inSLPM * SLPM_TO_CFM;
    } else if constexpr (toUnit == flow_unit_id_t::FLOW_UNIT_CIS) {
        return inSLPM * SLPM_TO_CIS;
    } else {
        return false;
    }
}

template<temp_unit_id_t fromUnit, temp_unit_id_t toUnit>
constexpr float ConvertTemperatureUnit(float temp)
{
    const float freezingPointF = 32.0F;
    const float boilingPointF = 212.0F;
    const float freezingPointC = 0.0F;
    const float boilingPointC = 100.0F;
    const float absoluteZeroK = 0.0F;
    const float freezingPointK = 273.15F;
    const float boilingPointK = 373.15F;

    float tempInCelsius;

    /// Convert the input temperature to Celsius
    if constexpr (fromUnit == temp_unit_id_t::TEMP_C) {
        tempInCelsius = temp;
    } else if constexpr (fromUnit == temp_unit_id_t::TEMP_F) {
        tempInCelsius = (temp - freezingPointF) * (boilingPointC - freezingPointC) / (boilingPointF - freezingPointF);
    } else if constexpr (fromUnit == temp_unit_id_t::TEMP_K) {
        tempInCelsius = temp - freezingPointK;
    } else {
        return false;
    }

    /// Convert the temperature in Celsius to the desired unit
    if constexpr (toUnit == temp_unit_id_t::TEMP_C) {
        return tempInCelsius;
    } else if constexpr (toUnit == temp_unit_id_t::TEMP_F) {
        return tempInCelsius * (boilingPointF - freezingPointF) / (boilingPointC - freezingPointC) + freezingPointF;
    } else if constexpr (toUnit == temp_unit_id_t::TEMP_K) {
        return tempInCelsius + freezingPointK;
    } else {
        return false;
    }
}

#endif  // UTILITY_H_
