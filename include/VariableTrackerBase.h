/**
 *
 * Nebula Tech Corporation
 *
 * Copyright © 2023 Nebula Tech Corporation. All Rights Reserved.
 * This file is part of HardFOC and is licensed under the GNU General Public License v3.0 or later.
 *
 */

#ifndef UTILITIES_COMMON_VARIABLETRACKERBASE_H_
#define UTILITIES_COMMON_VARIABLETRACKERBASE_H_

#include <cstdint>

/**
 * @enum SlopeCalculationType
 * @brief Specifies the type of slope calculation to be performed on a set of data points.
 *
 * This enumeration represents the various ways in which the slope, derived from a set of data points, can be calculated or represented:
 *
 * - AVERAGE: Represents the average slope across all data points.
 * - MAXIMUM: Represents the highest slope value among the data points.
 * - MINIMUM: Represents the lowest slope value among the data points.
 * - CHANGE: Represents the largest change between two consecutive slope values.
 * - LAST: Represents the most recent (or last) slope value from the data points.
 *
 * Depending on the use case, one may choose an appropriate `SlopeCalculationType` to get meaningful insights from the dataset.
 */
enum class SlopeCalculationType {
    AVERAGE,  ///< Average slope across all data points.
    MAXIMUM,  ///< Highest slope value among the data points.
    MINIMUM,  ///< Lowest slope value among the data points.
    CHANGE,   ///< Largest change between two consecutive slope values.
    LAST      ///< Most recent (or last) slope value from the data points.
};

/**
 * @brief Enumeration for different types of averaging schemes.
 */
enum class AveragingScheme {
    MEAN,       ///< Arithmetic mean
    MEDIAN,     ///< Median
    MODE,       ///< Mode
    GEOMETRIC,  ///< Geometric mean
    HARMONIC    ///< Harmonic mean
};

template <typename T>
class VariableTrackerBase {
public:
    virtual ~VariableTrackerBase() = default;

    /**
     * @brief Checks if the values have been consistently above or below a specified threshold for a given duration.
     *
     * This function evaluates the values stored in the `values` deque to determine if they have consistently
     * remained above or below a given threshold for a specified duration. The direction (above or below) is
     * dictated by the `checkBelow` argument.
     *
     * @note This function iterates through the values starting from the most recent ones and stops checking once
     * it goes beyond the specified duration. The available data should span at least the specified duration for
     * this function to return true.
     *
     * @param checkBelow If set to true, the function checks for values consistently below the threshold.
     *                   If set to false, it checks for values consistently above the threshold.
     * @param thresholdValue The threshold value against which the data is compared.
     * @param durationMsec The duration (in milliseconds) over which the values should be consistently above or below the threshold.
     * @param useCurrentTime If set to true (default), the function uses the current time to determine the end of the duration.
     *                       If set to false, it uses the timestamp of the newest value in the `values` deque.
     * @param minDataPoints Minimum data points required to be in the duration for checking. (default = 2)
     *
     * @return Returns true if the values within the specified duration have been consistently above or below the threshold,
     * and if there's enough data that spans the specified duration. Returns false otherwise.
     */
    virtual bool CheckIfValueConsistently(bool checkBelow, T thresholdValue, uint32_t durationMsec, bool useCurrentTime = true, uint32_t minDataPoints = 2) noexcept = 0;

    /**
     * @brief Checks if the values have been consistently between the given threshold for the given duration.
     *
     * This function evaluates the values stored in the `values` deque to determine if they have consistently
     * remained between the given thresholds for a specified duration.
     *
     * @note This function iterates through the values starting from the most recent ones and stops checking once
     * it goes beyond the specified duration. The available data should span at least the specified duration for
     * this function to return true.
     *
     * @param lowerThresholdValue The lower bound threshold value against which the data is compared.
     * @param upperThresholdValue The higher bound threshold value against which the data is compared.
     * @param durationMsec The duration (in milliseconds) over which the values should be consistently above or below the threshold.
     * @param useCurrentTime If set to true (default), the function uses the current time to determine the end of the duration.
     *                       If set to false, it uses the timestamp of the newest value in the `values` deque.
     * @param minDataPoints Minimum data points required to be in the duration for checking. (default = 2)
     *
     * @return Returns true if the values within the specified duration have been consistently between the thresholds,
     * and if there's enough data that spans the specified duration. Returns false otherwise.
     */
    virtual bool CheckIfValueBetweenBoundConsistently(T lowerThresholdValue, T upperThresholdValue, uint32_t durationMsec, bool useCurrentTime, uint32_t minDataPoints = 2) noexcept = 0;

    /**
     * @brief Checks if the values have been consistently out of the given thresholds for the given duration.
     *
     * This function evaluates the values stored in the `values` deque to determine if they have consistently
     * remained outside of the given thresholds for a specified duration.
     *
     * @note This function iterates through the values starting from the most recent ones and stops checking once
     * it goes beyond the specified duration. The available data should span at least the specified duration for
     * this function to return true.
     *
     * @param lowerThresholdValue The lower bound threshold value against which the data is compared.
     * @param upperThresholdValue The higher bound threshold value against which the data is compared.
     * @param durationMsec The duration (in milliseconds) over which the values should be consistently above or below the threshold.
     * @param useCurrentTime If set to true (default), the function uses the current time to determine the end of the duration.
     *                       If set to false, it uses the timestamp of the newest value in the `values` deque.
     * @param minDataPoints Minimum data points required to be in the duration for checking. (default = 2)
     *
     * @return Returns true if the values within the specified duration have been consistently between the thresholds,
     * and if there's enough data that spans the specified duration. Returns false otherwise.
     */
    virtual bool CheckIfValueOutOfBoundConsistently(T lowerThresholdValue, T upperThresholdValue, uint32_t durationMsec, bool useCurrentTime, uint32_t minDataPoints = 2) noexcept = 0;

    /**
     * @brief Calculates the tracked variable average value for the given duration with the specified AveragingScheme.
     *
     * This function evaluates the values stored in the `values` deque to caclulate the average value
     * of the data points that were stored in the specified duration.
     *
     * @note This function iterates through the values starting from the most recent ones and stops checking once
     * it goes beyond the specified duration. The available data should span at least the specified duration for
     * this function to return true.
     *
     * @param averageValue Reference variable of where to store calculated average value.
     * @param scheme The averaging scheme to use.
     * @param durationMsec The duration (in milliseconds) over which the values should exist over to be used in averaging.
     * @param useCurrentTime If set to true (default), the function uses the current time to determine the end of the duration.
     *                       If set to false, it uses the timestamp of the newest value in the `values` deque.
     * @param minDataPoints Minimum data points required to be in the duration for checking. (default = 2)
     *
     * @return Returns true if the values the minimum data points is satisfied, and if there's enough data
     * 			that spans the specified duration. Returns false otherwise.
     */
    virtual bool GetAverageSchemeValue(T& averageValue, AveragingScheme scheme, uint32_t durationMsec, bool useCurrentTime, uint32_t minDataPoints = 2) noexcept = 0;

    /**
     * @brief Computes the simple slope of the data values over a specified time duration.
     *
     * This function calculates the slope of the data values over a given time duration. The slope is computed
     * from the oldest data point within the time window to either the current time or the last recorded data point's
     * timestamp, based on the `useCurrentTime` parameter.
     *
     * @tparam T The data type of the values being monitored.
     *
     * @param deltaTimeMsec Duration in milliseconds over which the slope is to be calculated.
     *
     * @param calculatedSlope A reference where the computed slope will be stored if the calculation is successful.
     *
     * @param useCurrentTime Optional. A boolean to decide the end time of the checking window.
     *                       - true: Uses the current system time as the end time for the window (default).
     *                       - false: Uses the timestamp of the last recorded value as the end time.
     *
     * @return true If the slope calculation was successful.
     * @return false Otherwise, e.g., if there are not enough data points to calculate the slope.
     */
    virtual bool GetSimpleSlopeOverDeltaTime(uint32_t deltaTimeMsec, double &calculatedSlope, bool useCurrentTime = true) noexcept = 0;

    /**
     * @brief Retrieves the slope based on the given delta time and calculation type.
     *
     * Averages values within a short window and computes the rate of change of this average as the window moves through the dataset.
     * Then, the slope array is analyzed per the calculation type specified in arguments and is returned to caller through reference.
     *
     * @param deltaTimeMsec Time window (in milliseconds) to calculate the slope over.
     * @param[out] resultSlope The computed slope value will be stored in this reference variable.
     * @param calcType The type of slope calculation to perform.
     * @param windowSize Size of the moving average window. Defaults to 3.
     *
     * @return true If the slope calculation was successful.
     * @return false If there was insufficient data or any other error preventing the slope calculation.
     */
    virtual bool GetAdvancedSlopeOverDeltaTime(uint32_t deltaTimeMsec, double &resultSlope, SlopeCalculationType calcType = SlopeCalculationType::AVERAGE, uint32_t windowSize = 3) noexcept = 0;

    virtual bool IsValueStabilizedInMaxErrorBoundOverDeltaTime(float errorBound, uint32_t durationMsec, uint32_t minDataPoints = 2, bool verbose = false) {
    	T averageValue;
    	AveragingScheme scheme = AveragingScheme::MEAN;
    	bool useCurrentTime = true;
    	bool valueConsistentlyInBound = false;

    	/// If we're able to get average value in duration given with min data points passed in.
    	if(GetAverageSchemeValue(averageValue, scheme, durationMsec, useCurrentTime, minDataPoints))
    	{
    		/// Calculate lower and upper threshold data points need to be from average value
    		T lowerThresholdValue = static_cast<T>(static_cast<float>(averageValue) - errorBound/2.0F);
    		T upperThresholdValue = static_cast<T>(static_cast<float>(averageValue) + errorBound/2.0F);

    		/// Check if the past values have been around this average for the given duration, if so the flow is stabilized in given parameters.
    		valueConsistentlyInBound = CheckIfValueBetweenBoundConsistently(lowerThresholdValue, upperThresholdValue, durationMsec, useCurrentTime, minDataPoints);
    	}

    	return valueConsistentlyInBound;
    }
};

#endif /* UTILITIES_COMMON_VARIABLETRACKERBASE_H_ */
