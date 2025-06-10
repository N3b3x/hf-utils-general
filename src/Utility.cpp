/**
  * Nebula Tech Corporation
  *
  * Copyright © 2023 Nebula Tech Corporation.   All Rights Reserved.
 * This file is part of HardFOC and is licensed under the GNU General Public License v3.0 or later.
  *
  *  Contains the declaration and definition of the utility functions for general use.
  *
  *   Note:  These functions are not thread or interrupt-safe and should be called
 *          called with appropriate guards if used within an ISR or shared between tasks.
 */

#include "Utility.h"
#include "platform_compat.h"

#include <string>
#include <algorithm> // For std::transform
#include <cctype>    // For ::tolower, ::toupper
#include <sstream>
#include <vector>

/**
 * @brief Converts a string to lowercase.
 *
 * This function takes a string and converts all its characters to lowercase.
 *
 * @param str The input string to be converted.
 * @return A new string with all characters in lowercase.
 */
std::string StringToLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

/**
 * @brief Converts a string to uppercase.
 *
 * This function takes a string and converts all its characters to uppercase.
 *
 * @param str The input string to be converted.
 * @return A new string with all characters in uppercase.
 */
std::string StringToUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

/**
 * @brief Trims whitespace from both ends of a string.
 *
 * This function removes leading and trailing whitespace characters from the input string.
 *
 * @param str The input string to be trimmed.
 * @return A new string with leading and trailing whitespace removed.
 */
std::string StringTrim(const std::string& str) {
    std::string result = str;
    auto it = std::find_if(result.begin(), result.end(), [](char& ch){
        return !std::isspace(ch);
    });
    result.erase(result.begin(), it);
    auto rit = std::find_if(result.rbegin(), result.rend(), [](char& ch) {
        return !std::isspace(ch);
    });
    result.erase(rit.base(), result.end());
    return result;
}

/**
 * @brief Splits a string into a vector of strings using a specified delimiter.
 *
 * This function takes a string and splits it into a vector of substrings based on the specified delimiter.
 *
 * @param str The input string to be split.
 * @param delimiter The character used to split the string.
 * @return A vector of substrings.
 */
std::vector<std::string> StringSplit(const std::string& str, char delimiter) {
    std::vector<std::string> tokens; ///< Vector to store the resulting substrings

    std::istringstream tokenStream(str); ///< Input string stream to tokenize the input string
    std::string token;               ///< Variable to hold each substring during splitting

    /// Split the string based on the delimiter
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token); ///< Add each substring to the vector
    }

    return tokens; ///< Return the vector of substrings
}


bool ConvertTime(float inputValue, TimeUnit inputUnit, float &outputValue, TimeUnit outputUnit) {
    // Convert the input value to a base unit (let's use seconds as base)
    switch(inputUnit) {
        case NANOSECONDS:  inputValue *= 1e-9F; break;
        case MICROSECONDS: inputValue *= 1e-6F; break;
        case MILLISECONDS: inputValue *= 1e-3F; break;
        case SECONDS:      break; // Already in seconds
        case MINUTES:      inputValue *= 60; break;
        case HOURS:        inputValue *= 3600; break;
        case DAYS:         inputValue *= 86400; break;
        case YEARS:        inputValue *= 31536000; break; // Approximate (not accounting for leap years)
        default: return false;
    }

    // Convert the base value to the desired output unit
    switch(outputUnit) {
        case NANOSECONDS:  outputValue = inputValue / 1e-9F; break;
        case MICROSECONDS: outputValue = inputValue / 1e-6F; break;
        case MILLISECONDS: outputValue = inputValue / 1e-3F; break;
        case SECONDS:      outputValue = inputValue; break;
        case MINUTES:      outputValue = inputValue / 60; break;
        case HOURS:        outputValue = inputValue / 3600; break;
        case DAYS:         outputValue = inputValue / 86400; break;
        case YEARS:        outputValue = inputValue / 31536000; break;
        default: return false;
    }

    return true;
}



bool ConvertPressureUnit(float pressure, pressure_unit_id_t fromUnit, float &outputValue, pressure_unit_id_t toUnit)
{
	/// Conversion constants
	const float PA_TO_PA = 1.0f;
	const float PA_TO_PSI = 0.000145038f;
	const float PA_TO_BAR = 0.00001f;
	const float PA_TO_ATM = 0.00000986923f;
	const float PA_TO_MMHG = 0.00750062f;
	const float PA_TO_INHG = 0.02953f;
	const float PA_TO_MBAR = 0.01f;

	float inPascals = 0.0;

	/// Convert from the fromUnit to Pascal
	switch(fromUnit) {
	case PRESSURE_UNIT_PSI:
		inPascals = pressure / PA_TO_PSI;
		break;
	case PRESSURE_UNIT_PA:
		inPascals = pressure * PA_TO_PA;
		break;
	case PRESSURE_UNIT_BAR:
		inPascals = pressure / PA_TO_BAR;
		break;
	case PRESSURE_UNIT_ATM:
		inPascals = pressure / PA_TO_ATM;
		break;
	case PRESSURE_UNIT_MMHG:
		inPascals = pressure / PA_TO_MMHG;
		break;
	case PRESSURE_UNIT_INHG:
		inPascals = pressure / PA_TO_INHG;
		break;
	case PRESSURE_UNIT_MBAR:
		inPascals = pressure / PA_TO_MBAR;
		break;
	default:
        return false;
        break;
	}

	/// Convert from Pascal to the toUnit
	switch(toUnit) {
	case PRESSURE_UNIT_PSI:
		outputValue = inPascals * PA_TO_PSI;
        break;
	case PRESSURE_UNIT_PA:
		outputValue = inPascals * PA_TO_PA;
        break;
	case PRESSURE_UNIT_BAR:
		outputValue = inPascals * PA_TO_BAR;
        break;
	case PRESSURE_UNIT_ATM:
		outputValue = inPascals * PA_TO_ATM;
        break;
	case PRESSURE_UNIT_MMHG:
		outputValue = inPascals * PA_TO_MMHG;
        break;
	case PRESSURE_UNIT_INHG:
		outputValue = inPascals * PA_TO_INHG;
        break;
	case PRESSURE_UNIT_MBAR:
		outputValue = inPascals * PA_TO_MBAR;
        break;
	default:
        return false;
        break;
	}

	return true;
}

bool ConvertFlowUnit(float flow, flow_unit_id_t fromUnit, float &outputValue, flow_unit_id_t toUnit)
{
    /// Conversion constants
    const float SLPM_TO_SLPM = 1.0F;
    const float SLPM_TO_CMH = 60.0F;
    const float SLPM_TO_CFM = 0.0353147F;
    const float SLPM_TO_CIS = 2.11888F;

    float inSLPM = 0.0;

    /// Convert from the fromUnit to SLPM
    switch(fromUnit) {
    case FLOW_UNIT_SLPM:
        inSLPM = flow * SLPM_TO_SLPM;
        break;
    case FLOW_UNIT_CMH:
        inSLPM = flow / SLPM_TO_CMH;
        break;
    case FLOW_UNIT_CFM:
        inSLPM = flow / SLPM_TO_CFM;
        break;
    case FLOW_UNIT_CIS:
        inSLPM = flow / SLPM_TO_CIS;
        break;
    default:
        return false;
        break;
    }

    /// Convert from SLPM to the toUnit
    switch(toUnit) {
    case FLOW_UNIT_SLPM:
        outputValue = inSLPM * SLPM_TO_SLPM;
        break;
    case FLOW_UNIT_CMH:
        outputValue = inSLPM * SLPM_TO_CMH;
        break;
    case FLOW_UNIT_CFM:
        outputValue = inSLPM * SLPM_TO_CFM;
        break;
    case FLOW_UNIT_CIS:
        outputValue = inSLPM * SLPM_TO_CIS;
        break;
    default:
        return false;
        break;
    }

    return true;
}

bool ConvertTemperatureUnit(float temp, temp_unit_id_t fromUnit, float &outputValue, temp_unit_id_t toUnit)
{
    const float freezingPointF = 32.0F;
    const float boilingPointF = 212.0F;
    const float freezingPointC = 0.0F;
    const float boilingPointC = 100.0F;
    const float absoluteZeroK = 0.0F;
    const float freezingPointK = 273.15F;
    const float boilingPointK = 373.15F;

    float tempInCelsius;

    /// To stop [-Wunused-variable] from complaining
    UTIL_UNUSED(absoluteZeroK);
    UTIL_UNUSED(boilingPointK);

    /// Convert the input temperature to Celsius
    switch (fromUnit)
    {
        case TEMP_C:
            tempInCelsius = temp;
            break;
        case TEMP_F:
            tempInCelsius = (temp - freezingPointF) * (boilingPointC - freezingPointC) / (boilingPointF - freezingPointF);
            break;
        case TEMP_K:
            tempInCelsius = temp - freezingPointK;
            break;
        default:
            return false;
            break;
    }

    float convertedTemp;

    /// Convert the temperature in Celsius to the desired unit
    switch (toUnit)
    {
        case TEMP_C:
            convertedTemp = tempInCelsius;
            break;
        case TEMP_F:
            convertedTemp = tempInCelsius * (boilingPointF - freezingPointF) / (boilingPointC - freezingPointC) + freezingPointF;
            break;
        case TEMP_K:
            convertedTemp = tempInCelsius + freezingPointK;
            break;
        default:
            return false;
            break;
    }

    outputValue = convertedTemp;
    return true;
}


bool TestLogicWithTimeout(const std::function<bool()>& logic, bool expected, uint32_t timeoutMs, uint32_t timeBetweenChecksMs, uint32_t *pTimeTakenSav) {
    uint32_t startTime = GetElapsedTimeMsec();

    bool status = false;

    /// If no wait specified, just evaluate and leave
    if(timeoutMs == NO_WAIT) {
    	if (logic() == expected) {
    		status = true;
		}
    }

    /// Otherwise, evaluate in a while loop.
    else {
        uint64_t endTime = static_cast<uint64_t>(startTime) + static_cast<uint64_t>(timeoutMs);
        while (GetElapsedTimeMsec() <= endTime) {
            if (logic() == expected) {
                status = true;
                break;
            }
            uint32_t waitEnd = GetElapsedTimeMsec() + static_cast<uint16_t>(CONSTRAIN(timeBetweenChecksMs, 1, timeoutMs));
            while (GetElapsedTimeMsec() < waitEnd) {}
        }
    }

	if(pTimeTakenSav) {*pTimeTakenSav = (GetElapsedTimeMsec()-startTime);} /// Store time taken (calculated for start time to end)

    /// If we timed-out logic still not what's expected so return false.
    return status;
}

int32_t TwosCompliment( uint32_t value, uint8_t msb ) noexcept
{
    if( msb > 0 )
    {
       if( value & Bitmask(msb) )  // negative value?
       {
           value &= FullBitmask(msb-1);  // ignore the sign bit.
           return static_cast<int32_t>( value ) - Bitmask(msb);   // twos compliment conversion
       }
       else
       {
           value &= FullBitmask(msb );  // Ignore any bits greater than msb
           return static_cast<int32_t>(value);
       }

    }
    return 0;
}


uint32_t TwosComplimentFrom( int32_t value, uint8_t msb ) noexcept
{
    uint32_t clippedValue = static_cast<uint32_t>(value);
    clippedValue &= FullBitmask(msb );
    return clippedValue;
}

