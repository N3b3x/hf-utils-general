/**
 * @file ValueMonitor.h
 * @brief Monitors values for anomalies based on slope and threshold criteria.
 *
 * This module provides functionality to detect characteristics and anomalies in a time bounded
 * sequence of values. Characteristics include:
 * 1.  Average value
 * 2.  Minimum Value
 * 3.  Maximum Value
 * Anomalies can be of two types:
 * 1. Slope Anomalies: Detected based on the rate of change of values over a specified time window.
 * 2. Threshold Anomalies: Detected when values breach a predefined threshold.
 *
 * The monitoring system keeps track of the most recent anomalies and clears historical data
 * once non-anomalous values are received. This ensures focus on the most recent anomalies and reduces memory usage.
 * @section
 * The characteristics are
 * @section Slope Anomalies
 * The slope is calculated using the difference in values between the oldest and the newest data points
 * in the time window and dividing it by the time difference between these points. If this calculated slope
 * exceeds a predefined limit, it's considered a slope anomaly. Once a new value arrives that results in a slope
 * within the acceptable range, the record of past slope anomalies is cleared.
 *
 * @section Threshold Anomalies
 * Threshold anomalies are detected when values are either above or below a predefined threshold, depending on the configuration.
 * Once a new value arrives that's within the acceptable range, the record of past threshold anomalies is cleared.
 *
 * @note The emphasis of this monitoring system is on the most recent data. Historical anomalies are discarded once new
 * non-anomalous data is received.
 *
 * @tparam T The data type of the variable being monitored (e.g., int, double).
 *
 * @section Functionalities
 * - `CheckIfValueConsistently`: Checks if the values have been consistently above or below a specified threshold for a given duration.
 * - `GetSimpleSlopeOverDeltaTime`: Calculates the slope between the oldest and newest data points within a specified time range.
 * - `GetAdvancedSlopeOverDeltaTime`: Retrieves the slope based on the given delta time and calculation type.
 */


#ifndef VariableMonitor_H
#define VariableMonitor_H

#include <utility>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <vector>
#include "Utility.h"
#include "platform_compat.h"

#include "VariableTrackerBase.h"
#include "RingBuffer.h"
#include "TimestampedVariable.h"


enum class AnomalyType : uint8_t
{
	BelowLimit,   // Values below limit are anomalies
	AboveLimit    // Values above limit are anomalies
};

enum class SlopeType : uint8_t
{
	Absolute,     // Slope anomalies based on absolute value
	Directional  // Slope anomalies based on computed value
};

/**
* @brief This helper function simplifies the template code.
*
* @param deltaValue Change in value over the measured period
* @param deltaTimeMsec Time period (in milliseconds) to calculate slope over (if zero, function will return false)
* @param slopeLimit  Variable change over time that is considered
* @param slopeType Absolute or Directional.
* @param slopeAnomalyType : Above or below limit
* @return true if value meets the anomaly threshold, false otherwise.
*/
bool IsSlopeAnomaly( float deltaValue, float deltaTimeMsec, float slopeLimit, SlopeType slopeType, AnomalyType slopeAnomalyType ) noexcept;

/**
 * @brief A class to monitor a continuously updated variable and notify
 * when the slope exceeds a limit or the value crosses a threshold.
 */
template<typename Type, uint32_t MinTimeBetweenSamplesMsec, uint32_t SampleWindowMsec, uint32_t ThresholdWindowMsec, uint32_t SlopeWindowMsec = 0>
class VariableMonitor final : public VariableTrackerBase<Type>
{
public:

    /**
     * @brief Construct a new Variable Anomaly Monitor object to monitor variable changes based on threshold and slope.
     *
     * This constructor initializes the various settings for threshold and slope-based anomaly detection.
     *
     * @param threshold Initial value for the threshold against which values will be compared.
     * @param thresholdTimePeriodMsec Time period (in milliseconds) within which threshold-based anomaly detection is performed.
     * @param thresholdAnomalyDurationMsec Duration (in milliseconds) that a threshold anomaly must last to be considered significant.
     * @param checkBelowThreshold Optional. Specifies whether to check for values below the threshold (default is true).
     * @param slopeLimit Initial value for the slope limit against which the rate of change of values will be compared.
     * @param slopeTimePeriodMsec Time period (in milliseconds) within which slope-based anomaly detection is performed.
     * @param slopeAnomalyDurationMsec Duration (in milliseconds) that a slope anomaly must last to be considered significant.
     * @param checkBelowSlopeThreshold Optional. Specifies whether to check for slope values below the slope limit (default is false).
     * @param useAbsoluteSlope Optional. Specifies whether to use the absolute values for both the calculated slope and the slope limit (default is true).
     */
    VariableMonitor(Type threshold,	AnomalyType thresholdAnomalyType = AnomalyType::AboveLimit,
		float slopeLimit = 0.0, AnomalyType slopeAnomalyType = AnomalyType::AboveLimit, SlopeType slopeType = SlopeType::Absolute) noexcept;

    /**
     * @brief Update the collection with a new value.
     * This should be called every time the variable being monitored changes.
     *
     * @param newValue The new value of the variable.
     *
     * @return True if the data was actually stored in tracker.
     * @return False if the data was not stored due to violating minTimeBetweenSampleStoreMsec.
     */
    bool UpdateValue(Type newValue) noexcept;

    /**
     * @brief Gets the most recent value stored.
     * @param timestamp Optional pointer to a variable where the timestamp of the most recent value will be stored. If not provided, the timestamp is ignored.
     * @return The most recent value in the window of type T.
     * @exception noexcept This function does not throw exceptions.
     */
    Type GetLastValue(uint32_t *timestamp = nullptr) const noexcept;

    /**
     * @brief Gets the most recent value stored.
     * @param value Reference to a variable where the most recent value will be stored.
     * @param timestamp Optional pointer to a variable where the timestamp of the most recent value will be stored. If not provided, the timestamp is ignored.
     * @return True if the value was successfully retrieved, false otherwise.
     * @exception noexcept This function does not throw exceptions.
     */
    bool GetLastValue(Type& value, uint32_t *timestamp) const noexcept;


    /**
	 * @brief Gets the maximum value in the window.
	 * @param value  The largest value in the window.
	 * @return true if value available, false otherwise.
	 */
    bool GetMaxValue( Type& value ) const noexcept;

    /**
	  * @brief Gets the minimum value in the deque.
	  * @param value  The smallest value in the deque.
	  *  @return true if value available, false otherwise.
	  */
	bool GetMinValue( Type& value ) const noexcept;


	/// Constants to determine the size of buffers.  A buffer can be zero length

	static constexpr uint32_t ValueCount = (SampleWindowMsec / MinTimeBetweenSamplesMsec) + 1;
	static constexpr uint32_t ThresholdAnomalyCount = ThresholdWindowMsec > 0 ? (ThresholdWindowMsec / MinTimeBetweenSamplesMsec) + 1 : 0;
	static constexpr uint32_t SlopeAnomalyCount = SlopeWindowMsec > 0 ? (SlopeWindowMsec/MinTimeBetweenSamplesMsec ) + 1 : 0 ;

	using ValueType = TimestampedVariable<Type>;
	using ValueBuffer = RingBuffer<TimestampedVariable<Type>, ValueCount>;
	using iterator =  typename ValueBuffer::iterator;
	using const_iterator =  typename ValueBuffer::const_iterator;


	/**
	* @brief Returns an iterator to the oldest entry in the buffer whose time stamp is greater than or equal to specified time
	*/
	iterator GetOldestEntry( uint32_t oldestTimestampMsec ) noexcept;

	const_iterator GetOldestEntry( uint32_t oldestTimestampMsec ) const noexcept;

	/**
	* @brief Returns an iterator to the newest entry in the buffer whose time stamp is less than or equal to the specified time
	*/
	iterator GetNewestEntry( uint32_t newestTimestampMsec ) noexcept;

	const_iterator GetNewestEntry( uint32_t newestTimestampMsec ) const noexcept;

	iterator GetEnd() noexcept
    {
    	return values.end();
    }

    const_iterator GetEnd() const noexcept
    {
       	return values.cend();
    }

    /**
     * @brief Set the limit for the variable's rate of change (slope) and
     * the time period over which to calculate it.
     *
     * @param slope The slope limit. The function checkSlope() will return true
     * if the variable's rate of change exceeds this limit.
     * @param timePeriodMsec The time period (in milliseconds) over which to calculate the slope.
     * @param anomalyDurationMsec The duration of time (in milliseconds) for which the anomaly must exist.
     */
//    void SetSlopeLimit(float slopeLimit, uint32_t timePeriodMsec, uint32_t anomalyDurationMsec);

    /**
     * @brief Sets the mode for slope anomaly detection to either use absolute values or not.
     *
     * When set to true, the function will use the absolute value of the calculated slope
     * and the slope limit for anomaly detection. This will detect rapid changes in the variable
     * regardless of the direction (increasing or decreasing).
     *
     * When set to false, the function will use the actual calculated slope and slope limit values,
     * allowing for direction-specific anomaly detection.
     *
     * @param useAbsolute A boolean flag indicating whether to use absolute values for slope anomaly detection.
     *                     - true: Use absolute values for both the calculated slope and the slope limit.
     *                     - false: Use the actual values for the calculated slope and the slope limit.
     */
 //   void UseAbsoluteSlope(bool useAbsolute);

    /**
     * @brief Set the threshold value
     * and the duration of time for which the anomaly must exist.
     *
     * @param threshold The threshold value. The function checkThreshold()
     *                  will return true if the variable's value is consistently on one side of this threshold for the set time period.
     */
  //  void SetThreshold(Type threshold);

    /**
     * @brief Set the threshold duration of time for which the anomaly must exist.
     *
     * @param anomalyDurationMsec The duration of time (in milliseconds) for which the anomaly must exist.
     */
//    void SetThresholdAnomalyDurationMsec(uint32_t anomalyDurationMsec);

    /**
     * @brief Set the threshold, the time period for threshold checking,
     * and the duration of time for which the anomaly must exist.
     *
     * @param threshold The threshold value. The function checkThreshold()
     * will return true if the variable's value is consistently on one side of this threshold for the set time period.
     * @param timePeriodMsec The time period (in milliseconds) over which to check if the variable is below or above the threshold.
     * @param anomalyDurationMsec The duration of time (in milliseconds) for which the anomaly must exist.
     */
 //   void SetThreshold(Type threshold, uint32_t timePeriodMsec, uint32_t anomalyDurationMsec);

    /**
     * @brief Set the Check Below Threshold flag
     *
     * @param checkBelowThresholdArg Specifies whether to check for values below or above the threshold.
     */
 //   void SetCheckBelowThreshold(bool checkBelowThresholdArg); // New member function

    /**
     * @brief Check if the variable has been consistently below or above
     * the threshold for the set time period.
     *
     * @return true If the variable has been below or above the threshold for the set time period
     * @return false Otherwise
     */
  //  bool CheckThreshold();

    /**
     * @brief Checks if the values have been consistently above or below a specified threshold for a given duration.
     *
     * This function evaluates the values stored in the `values` collection to determine if they have consistently
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
     bool CheckIfValueConsistently(bool checkBelow, Type thresholdValue, uint32_t durationMsec, bool useCurrentTime = true, uint32_t minDataPoints = 2) noexcept override;

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
    bool CheckIfValueBetweenBoundConsistently(Type lowerThresholdValue, Type upperThresholdValue, uint32_t durationMsec, bool useCurrentTime, uint32_t minDataPoints = 2) noexcept override;

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
    bool CheckIfValueOutOfBoundConsistently(Type lowerThresholdValue, Type upperThresholdValue, uint32_t durationMsec, bool useCurrentTime, uint32_t minDataPoints = 2) noexcept override;

    /**
     * @brief Calculates the tracked variable average value for the given duration.
     *
     * This function evaluates the values stored in the `values` deque to caclualte the average value
     * of the data points that were stored in the specified duration.
     *
     * @note This function iterates through the values starting from the most recent ones and stops checking once
     * it goes beyond the specified duration. The available data should span at least the specified duration for
     * this function to return true.
     *
     * @param averageValue Reference variable of where to store calculated average value.
     * @param durationMsec The duration (in milliseconds) over which the values should exist over to be used in averaging.
     * @param useCurrentTime If set to true (default), the function uses the current time to determine the end of the duration.
     *                       If set to false, it uses the timestamp of the newest value in the `values` deque.
     * @param minDataPoints Minimum data points required to be in the duration for checking. (default = 2)
     *
     * @return Returns true if the values the minimum data points is satisfied, and if there's enough data
     * 			that spans the specified duration. Returns false otherwise.
     */
    bool GetAverageValue(Type& averageValue, uint32_t durationMsec, bool useCurrentTime, uint32_t minDataPoints = 2) const noexcept;

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
    bool GetAverageSchemeValue(Type& averageValue, AveragingScheme scheme, uint32_t durationMsec, bool useCurrentTime, uint32_t minDataPoints = 2) noexcept override;

    /**
     * @brief Check if the variable's rate of change (slope) has exceeded
     * the limit within the set time period.
     *
     * @return true If the slope is greater than the limit
     * @return false Otherwise
     */
    bool CheckSlope() const ;

    /**
     * @brief Checks if the calculated slope within a specified duration exceeds a threshold in a certain direction.
     *
     * This function calculates the slope of the data values over a specified time duration and compares it against
     * a threshold to detect anomalies. The direction of the comparison (above or below the threshold) is determined
     * by the `checkBelow` parameter. Additionally, the function can use the absolute value of the slope and threshold
     * for comparison based on the `useAbsolute` parameter.
     *
     * @tparam T The data type of the values being monitored.
     *
     * @param checkBelow A boolean flag indicating the direction of the comparison.
     *                   - true: Checks if the slope is less than the threshold.
     *                   - false: Checks if the slope is greater than the threshold.
     *
     * @param slopeThresh The threshold value for the slope to be compared against.
     *
     * @param useAbsolute Optional. A boolean flag indicating whether to use absolute values for comparison.
     *                    - true: Use absolute values for both the slope and the threshold.
     *                    - false: Use the actual values for the slope and the threshold.
     *
     * @param deltaTimeMsec Duration in milliseconds over which the slope is to be calculated. This parameter
     *                      helps in determining the time range for which the slope is computed, allowing for
     *                      dynamic assessment of slopes over varying intervals.
     *
     * @return true If the slope meets the condition based on the provided parameters.
     * @return false Otherwise, or if the slope calculation was not successful.
     */
    bool CheckIfSlope(bool checkBelow, double slopeThresh, bool useAbsolute, uint32_t deltaTimeMsec, bool useCurrentTime = true) const;

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
    bool GetSimpleSlopeOverDeltaTime(uint32_t deltaTimeMsec, double &calculatedSlope, bool useCurrentTime = true) noexcept override;

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
    bool GetAdvancedSlopeOverDeltaTime(uint32_t deltaTimeMsec, double &resultSlope, SlopeCalculationType calcType = SlopeCalculationType::AVERAGE, uint32_t windowSize = 3) noexcept override;

    /**
     * @brief Retrieves the most recent time when a slope anomaly was detected.
     *
     * @return uint32_t The timestamp of the most recent slope anomaly acquired by GetElapsedTimeMsec().
     */
  //  uint32_t GetLastSlopeAnomalyTimeMsec() const;

    /**
     * @brief Retrieves the most recent time when a threshold anomaly was detected.
     *
     * @return uint32_t The timestamp of the most recent threshold anomaly.
     */
 //   uint32_t GetLastThresholdAnomalyTime() const;

    /**
     * @brief Removes old values and anomalies from the deques.
     */
//    void Cleanup();

    /**
     * @brief Removes all values and anomalies
     */
    void Erase() noexcept;

    /**
      * @brief Returns the number of values within the sample window.
      */
    uint32_t GetValueCount() const noexcept;

    /**
      * @brief Returns the number of entries within the threshold sample window that exceed the threshold criteria (above or below)
      */
    uint32_t GetThresholdAnomalyCount() const noexcept;

    /**
      * @brief Returns the number of entries within the slope sample window that exceed the slope criteria (above or below)
      */
    uint32_t GetSlopeAnomalyCount() const noexcept;

private:

    /**
     * @brief Calculates the slope result based on the given slopes and calculation type.
     *
     * @param slopes A collection of calculated slopes.
     * @param calcType The type of slope calculation to perform.
     * @param resultSlope Calculated slope based on the desired type.
     *
     * @return true If the slope calculation was successful, otherwise false.
     */
    bool CalculateSlopeFromType(const std::vector<double> &slopes, SlopeCalculationType calcType, double &resultSlope);

    ValueBuffer values;      ///< Buffer to store time-stamped value.

    Type threshold;                             ///< The threshold value.
    AnomalyType thresholdAnomalyType;           ///< True if checking for values below the threshold, false if checking for values above the threshold.
    RingBuffer<uint32_t, ThresholdAnomalyCount> thresholdAnomalies;    ///< Buffer to store the starting times of threshold anomalies.

    float slopeLimit;                          ///< The slope limit.
    AnomalyType slopeAnomalyType;              ///< True if checking for slope values below the limit, false if checking for values above the limit.
    SlopeType slopeType;
    RingBuffer<uint32_t, SlopeAnomalyCount> slopeAnomalies;    ///< Buffer to store the starting times of slope anomalies.
};


template<typename Type, uint32_t MinTimeBetweenSamplesMsec, uint32_t SampleWindowMsec, uint32_t ThresholdWindowMsec, uint32_t SlopeWindowMsec>
VariableMonitor<Type,MinTimeBetweenSamplesMsec,SampleWindowMsec, ThresholdWindowMsec,SlopeWindowMsec>::VariableMonitor(
		Type thresholdArg,	 AnomalyType thresholdAnomalyTypeArg,
                float slopeLimitArg, AnomalyType slopeAnomalyTypeArg, SlopeType slopeTypeArg) noexcept :
    values{},
    threshold(thresholdArg),
	thresholdAnomalyType(thresholdAnomalyTypeArg),
	thresholdAnomalies{},
    slopeLimit(slopeLimitArg),
	slopeAnomalyType(slopeAnomalyTypeArg),
	slopeType(slopeTypeArg),
	slopeAnomalies{}
{


    /// The constructor initializing all values. No logic is implemented here.
  //  values.fill( ValueType::UnusedValue );
}


/**
  * @brief Returns the number of values within the sample window.   Start at the most recent value and check moving backward
  */
template<typename Type, uint32_t MinTimeBetweenSamplesMsec, uint32_t SampleWindowMsec, uint32_t ThresholdWindowMsec, uint32_t SlopeWindowMsec>
uint32_t VariableMonitor<Type,MinTimeBetweenSamplesMsec,SampleWindowMsec, ThresholdWindowMsec,SlopeWindowMsec>::GetValueCount() const noexcept
{
	const uint32_t currentTimeMsec = GetElapsedTimeMsec();
	// This check covers the corner case of window bigger than time that has elapsed.
    const uint32_t oldestTimeMsec = (currentTimeMsec > SampleWindowMsec) ? GetElapsedTimeMsec() - SampleWindowMsec : 0;

	uint32_t count = 0;

	for( auto valueIterator = values.crbegin(); valueIterator != values.crend(); --valueIterator )
	{
		if( valueIterator->GetTimestamp() >= oldestTimeMsec )
		{
			++count;
		}
		else
		{
			break;
		}
	}
   	return count;
}


/**
  * @brief Returns the number of values within the threshold sample window that exceed the threshold criteria (above or below).  The thresholdAnomalies
  * contains the timestamps of the oit of range values, so just count the number of values that are out of range.  Start from the newest (most recent) value
  * and move backward in time.
  */
template<typename Type, uint32_t MinTimeBetweenSamplesMsec, uint32_t SampleWindowMsec, uint32_t ThresholdWindowMsec, uint32_t SlopeWindowMsec>
uint32_t VariableMonitor<Type,MinTimeBetweenSamplesMsec,SampleWindowMsec, ThresholdWindowMsec,SlopeWindowMsec>::GetThresholdAnomalyCount() const noexcept
{
	const uint32_t currentTimeMsec = GetElapsedTimeMsec();
	// This check covers the corner case of window bigger than time that has elapsed.
	const uint32_t oldestTimeMsec = (currentTimeMsec > ThresholdWindowMsec) ? GetElapsedTimeMsec() - ThresholdWindowMsec : 0;

	uint32_t count = 0;
	for( auto anomalyIterator = thresholdAnomalies.crbegin(); anomalyIterator != thresholdAnomalies.crend(); --anomalyIterator)
	{
		if( *anomalyIterator >= oldestTimeMsec )
		{
			++count;
		}
		else
		{
			break;
		}
	}

	return count;
}

/**
  * @brief Returns the number of values within the slope sample window that exceed the slope criteria (above or below)
  */

template<typename Type, uint32_t MinTimeBetweenSamplesMsec, uint32_t SampleWindowMsec, uint32_t ThresholdWindowMsec, uint32_t SlopeWindowMsec>
uint32_t VariableMonitor<Type,MinTimeBetweenSamplesMsec,SampleWindowMsec, ThresholdWindowMsec,SlopeWindowMsec>::GetSlopeAnomalyCount() const noexcept
{
	const uint32_t currentTimeMsec = GetElapsedTimeMsec();
	// This check covers the corner case of window bigger than time that has elapsed.
	const uint32_t oldestTimeMsec = (currentTimeMsec > SlopeWindowMsec) ? (currentTimeMsec - SlopeWindowMsec + 1) : 0;
	uint32_t count = 0;

	for( auto slopeIterator = slopeAnomalies.crbegin(); slopeIterator != slopeAnomalies.crend(); ++slopeIterator )
	{
		uint32_t runningTimestamp = *slopeIterator;
		if( runningTimestamp >= oldestTimeMsec )
		{
			++count;
		}
		else
		{
			break;
		}
	}

	return count;
}


/**
  * @brief Returns an iterator to the oldest value in the buffer whose timestamp that is equal to or greater than the specified value
  */

template<typename Type, uint32_t MinTimeBetweenSamplesMsec, uint32_t SampleWindowMsec, uint32_t ThresholdWindowMsec, uint32_t SlopeWindowMsec>
typename VariableMonitor<Type,MinTimeBetweenSamplesMsec,SampleWindowMsec, ThresholdWindowMsec,SlopeWindowMsec>::iterator
VariableMonitor<Type,MinTimeBetweenSamplesMsec,SampleWindowMsec, ThresholdWindowMsec,SlopeWindowMsec>::GetOldestEntry( uint32_t oldestTimestampMsec ) noexcept
{
	auto valueIterator = values.rbegin();   // Start at the most recent value
	if( valueIterator != values.end() )
	{
		auto priorValueIterator = valueIterator;
		priorValueIterator --;

		// Go backwards in time till we hit end of buffer or value that is too old.
		while( (priorValueIterator != values.end() ) && ( priorValueIterator->GetTimestamp() >= oldestTimestampMsec  ) )
		{
			valueIterator = priorValueIterator; // Keep the running value
			--priorValueIterator;
		}
	}
	return valueIterator;
}


template<typename Type, uint32_t MinTimeBetweenSamplesMsec, uint32_t SampleWindowMsec, uint32_t ThresholdWindowMsec, uint32_t SlopeWindowMsec>
typename VariableMonitor<Type,MinTimeBetweenSamplesMsec,SampleWindowMsec, ThresholdWindowMsec,SlopeWindowMsec>::const_iterator
VariableMonitor<Type,MinTimeBetweenSamplesMsec,SampleWindowMsec, ThresholdWindowMsec,SlopeWindowMsec>::GetOldestEntry( uint32_t oldestTimestampMsec ) const noexcept
{
	auto valueIterator = values.crbegin();   // Start at the most recent value
	if( valueIterator != values.crend() )
	{
		auto priorIterator = valueIterator;
		priorIterator--;

		// Go backwards in time till we hit end of buffer or value that is too old.
		while( (priorIterator != values.crend() ) && ( priorIterator->GetTimestamp() >= oldestTimestampMsec  ) )
		{
			valueIterator = priorIterator; // Keep the running value
			--priorIterator;
		}
	}
	return valueIterator;
}

template<typename Type, uint32_t MinTimeBetweenSamplesMsec, uint32_t SampleWindowMsec, uint32_t ThresholdWindowMsec, uint32_t SlopeWindowMsec>
typename VariableMonitor<Type,MinTimeBetweenSamplesMsec,SampleWindowMsec, ThresholdWindowMsec,SlopeWindowMsec>::iterator
VariableMonitor<Type,MinTimeBetweenSamplesMsec,SampleWindowMsec, ThresholdWindowMsec,SlopeWindowMsec>::GetNewestEntry( uint32_t newestTimestampMsec ) noexcept
{
	iterator valueIterator = values.rbegin();   // Start at the most recent value
	if( valueIterator != values.end() )
	{
		auto priorValueIterator = valueIterator;
		priorValueIterator --;

		// Go backwards in time till we hit end of buffer or value that is too old.
		while( (priorValueIterator != values.end() ) && ( priorValueIterator->GetTimestamp() > newestTimestampMsec  ) )
		{
			valueIterator = priorValueIterator; // Keep the running value
			--priorValueIterator;
		}
	}
	return valueIterator;
}

template<typename Type, uint32_t MinTimeBetweenSamplesMsec, uint32_t SampleWindowMsec, uint32_t ThresholdWindowMsec, uint32_t SlopeWindowMsec>
typename VariableMonitor<Type,MinTimeBetweenSamplesMsec,SampleWindowMsec, ThresholdWindowMsec,SlopeWindowMsec>::const_iterator
VariableMonitor<Type,MinTimeBetweenSamplesMsec,SampleWindowMsec, ThresholdWindowMsec,SlopeWindowMsec>::GetNewestEntry( uint32_t newestTimestampMsec ) const noexcept
{
	const_iterator valueIterator = values.crbegin();   // Start at the most recent value
	if( valueIterator != values.crend() )
	{
		auto priorIterator = valueIterator;
		priorIterator--;

		// Go backwards in time till we hit end of buffer or value that is too old.
		while( (priorIterator != values.crend() ) && ( priorIterator->GetTimestamp() > newestTimestampMsec  ) )
		{
			valueIterator = priorIterator; // Keep the running value
			--priorIterator;
		}
	}
	return valueIterator;
}

template<typename Type, uint32_t MinTimeBetweenSamplesMsec, uint32_t SampleWindowMsec, uint32_t ThresholdWindowMsec, uint32_t SlopeWindowMsec>
bool VariableMonitor<Type,MinTimeBetweenSamplesMsec,SampleWindowMsec, ThresholdWindowMsec,SlopeWindowMsec>::UpdateValue(Type newValue) noexcept
{
	const TimestampedVariable<Type> timestampedValue( newValue );   // Create a time-stamped variable

	auto valueIterator = values.rbegin();
	auto endIterator =  values.rend();
    if( valueIterator != endIterator ) // Make sure its not empty
    {
    	uint32_t minimumAddTimeMsec = valueIterator->GetTimestamp() + MinTimeBetweenSamplesMsec;

        if( timestampedValue.GetTimestamp() >= minimumAddTimeMsec ) /// Check current time against most recent addition
        {
        	values.Append(timestampedValue);
        }
        else // Update has occurred to quickly
        {
        	return false;
        }

        if( SlopeWindowMsec > 0 )  // Only check for slope anomalies if window is defined.
        {
        	// This check covers the corner case of window bigger than time that has elapsed.

        	uint32_t oldestTimestamp = (timestampedValue.GetTimestamp() >= SlopeWindowMsec ) ? (timestampedValue.GetTimestamp()- SlopeWindowMsec) : 0;
			auto startingValueIterator = GetOldestEntry(oldestTimestamp);
			uint32_t deltaTimeMsec = timestampedValue.GetTimestamp() - startingValueIterator->GetTimestamp();

			if( deltaTimeMsec >= std::max(SlopeWindowMsec,(uint32_t)0U) )
		    {
				const float deltaValue = static_cast<float>(timestampedValue.GetValue() - startingValueIterator->GetValue());
				const float deltaMsec = static_cast<float>(deltaTimeMsec);

				if( IsSlopeAnomaly(deltaValue, deltaMsec, slopeLimit, slopeType, slopeAnomalyType) )
				{
					slopeAnomalies.Append(timestampedValue.GetTimestamp());

				}
				else  // Not an anomaly, clear the slope anomaly buffer
				{
					slopeAnomalies.Erase();
				}
			}
		}
    }
    else  // first value being added, no need to check for slope issues
    {
         values.Append(timestampedValue);
    }

    if( ThresholdWindowMsec > 0 )  // Only check for threshold anomalies if window is defined.
	{
    	if( thresholdAnomalyType == AnomalyType::AboveLimit  ) /// Use absolute values for comparison
    	{
                if( timestampedValue.GetValue() > threshold )
                {
                        thresholdAnomalies.Append( timestampedValue.GetTimestamp());
                }
                else  // Not an anomaly, clear the threshold anomaly buffer
                {
                        thresholdAnomalies.Erase();
                }
    	}
    	else if( timestampedValue.GetValue() < threshold )
		{
    		thresholdAnomalies.Append(timestampedValue.GetTimestamp());
		}
    	else  // Not an anomaly, clear the threshold anomaly buffer
		{
    		thresholdAnomalies.Erase();
		}
	}

    return true;
}


template<typename Type, uint32_t MinTimeBetweenSamplesMsec, uint32_t SampleWindowMsec, uint32_t ThresholdWindowMsec, uint32_t SlopeWindowMsec>
Type VariableMonitor<Type,MinTimeBetweenSamplesMsec,SampleWindowMsec, ThresholdWindowMsec,SlopeWindowMsec>::GetLastValue(uint32_t *timestamp) const noexcept
{
	const uint32_t currentTimeMsec = GetElapsedTimeMsec();
	// This check covers the corner case of window bigger than time that has elapsed.
	const uint32_t oldestTimeMsec = (currentTimeMsec > SampleWindowMsec) ? GetElapsedTimeMsec() - SampleWindowMsec : 0;

	auto valueIterator = values.crbegin();

	if( valueIterator != values.crend() )
	{
		 if( valueIterator->GetTimestamp() >= oldestTimeMsec)
		 {
			 if(timestamp) { *timestamp = valueIterator->GetTimestamp(); }
			 return valueIterator->GetValue();
		 }
	}

	//if(timestamp) { *timestamp = GetElapsedTimeMsec(); }
	return Type{};
}

template<typename Type, uint32_t MinTimeBetweenSamplesMsec, uint32_t SampleWindowMsec, uint32_t ThresholdWindowMsec, uint32_t SlopeWindowMsec>
bool VariableMonitor<Type,MinTimeBetweenSamplesMsec,SampleWindowMsec, ThresholdWindowMsec,SlopeWindowMsec>::GetLastValue(Type& value, uint32_t *timestamp) const noexcept
{
	const uint32_t currentTimeMsec = GetElapsedTimeMsec();
	// This check covers the corner case of window bigger than time that has elapsed.
	const uint32_t oldestTimeMsec = (currentTimeMsec > SampleWindowMsec) ? GetElapsedTimeMsec() - SampleWindowMsec : 0;

	auto valueIterator = values.crbegin();

	if( valueIterator != values.crend() )
	{
		 if( valueIterator->GetTimestamp() >= oldestTimeMsec)
		 {
			 if(timestamp) { *timestamp = valueIterator->GetTimestamp(); }
			 value = valueIterator->GetValue();
			 return true;
		 }
	}

	return false;
}


template<typename Type, uint32_t MinTimeBetweenSamplesMsec, uint32_t SampleWindowMsec, uint32_t ThresholdWindowMsec, uint32_t SlopeWindowMsec>
bool VariableMonitor<Type,MinTimeBetweenSamplesMsec,SampleWindowMsec, ThresholdWindowMsec,SlopeWindowMsec>::GetMaxValue( Type& value ) const noexcept
{
	const uint32_t currentTimeMsec = GetElapsedTimeMsec();
	// This check covers the corner case of window bigger than time that has elapsed.
	const uint32_t oldestTimeMsec = (currentTimeMsec > SampleWindowMsec) ? GetElapsedTimeMsec() - SampleWindowMsec : 0;

	auto valueIterator = values.crbegin();

	if( valueIterator != values.crend() )
	{
		 if( valueIterator->GetTimestamp() >= oldestTimeMsec)
		 {
			 value = valueIterator->GetValue();
			 while( (--valueIterator != values.crend() ) && ( valueIterator->GetTimestamp() >= oldestTimeMsec) )
			 {
		         value = std::max( value, valueIterator->GetValue());
			 }
			 return true;
		 }
	}

    return false;
}

template<typename Type, uint32_t MinTimeBetweenSamplesMsec, uint32_t SampleWindowMsec, uint32_t ThresholdWindowMsec, uint32_t SlopeWindowMsec>
bool VariableMonitor<Type,MinTimeBetweenSamplesMsec,SampleWindowMsec, ThresholdWindowMsec,SlopeWindowMsec>::GetMinValue( Type& value ) const noexcept
{
	const uint32_t currentTimeMsec = GetElapsedTimeMsec();
	// This check covers the corner case of window bigger than time that has elapsed.
	const uint32_t oldestTimeMsec = (currentTimeMsec > SampleWindowMsec) ? GetElapsedTimeMsec() - SampleWindowMsec : 0;

	auto valueIterator = values.crbegin();

	if( valueIterator != values.crend() )
	{
		 if( valueIterator->GetTimestamp() >= oldestTimeMsec)
		 {
			 value = valueIterator->GetValue();
			 while( (--valueIterator != values.crend() ) && ( valueIterator->GetTimestamp() >= oldestTimeMsec) )
			 {
				 value = std::min( value, valueIterator->GetValue());
			 }
			 return true;
		 }
	}
    return false;
}

/*
template<typename Type, uint32_t MinTimeBetweenSamplesMsec, uint32_t SampleWindowMsec, uint32_t ThresholdWindowMsec, uint32_t SlopeWindowMsec>
void VariableMonitor<Type,MinTimeBetweenSamplesMsec,SampleWindowMsec, ThresholdWindowMsec,SlopeWindowMsec>::SetMinTimeBetweenUpdateStoreMsec(uint32_t minTimeBetweenUpdateMsec)
{
    minTimeBetweenUpdateSampleStoringMsec = minTimeBetweenUpdateMsec;
}



template<typename Type, uint32_t MinTimeBetweenSamplesMsec, uint32_t SampleWindowMsec, uint32_t ThresholdWindowMsec, uint32_t SlopeWindowMsec>
void VariableMonitor<Type,MinTimeBetweenSamplesMsec,SampleWindowMsec, ThresholdWindowMsec,SlopeWindowMsec>::SetSlopeLimit(double slope, uint32_t timePeriodMsec, uint32_t anomalyDurationMsec) {

    /// Setting the slope limit, time period for slope calculation and duration of time for which the slope anomaly must exist.
    slopeLimit = slope;
    slopeTimePeriodMsec = timePeriodMsec;
    slopeAnomalyDurationMsec = anomalyDurationMsec;
}



template<typename Type, uint32_t MinTimeBetweenSamplesMsec, uint32_t SampleWindowMsec, uint32_t ThresholdWindowMsec, uint32_t SlopeWindowMsec>
void VariableMonitor<Type,MinTimeBetweenSamplesMsec,SampleWindowMsec, ThresholdWindowMsec,SlopeWindowMsec>::UseAbsoluteSlope(bool useAbsolute) {
    useAbsoluteSlope = useAbsolute;
}

template<typename Type, uint32_t MinTimeBetweenSamplesMsec, uint32_t SampleWindowMsec, uint32_t ThresholdWindowMsec, uint32_t SlopeWindowMsec>
void VariableMonitor<Type,MinTimeBetweenSamplesMsec,SampleWindowMsec, ThresholdWindowMsec,SlopeWindowMsec>::SetThreshold(Type newThreshold, uint32_t timePeriodMsec, uint32_t anomalyDurationMsec) {

    /// Setting the threshold, time period for threshold checking and duration of time for which the threshold anomaly must exist.
    threshold = newThreshold;
    thresholdAnomalyDurationMsec = anomalyDurationMsec;
    thresholdTimePeriodMsec = timePeriodMsec;
}



template<typename Type, uint32_t MinTimeBetweenSamplesMsec, uint32_t SampleWindowMsec, uint32_t ThresholdWindowMsec, uint32_t SlopeWindowMsec>
void VariableMonitor<Type,MinTimeBetweenSamplesMsec,SampleWindowMsec, ThresholdWindowMsec,SlopeWindowMsec>::SetThreshold(T newThreshold) {

    /// Setting the threshold, time period for threshold.
    threshold = newThreshold;
}

template<typename Type, uint32_t MinTimeBetweenSamplesMsec, uint32_t SampleWindowMsec, uint32_t ThresholdWindowMsec, uint32_t SlopeWindowMsec>
void VariableMonitor<Type,MinTimeBetweenSamplesMsec,SampleWindowMsec, ThresholdWindowMsec,SlopeWindowMsec>::SetThresholdAnomalyDurationMsec(uint32_t anomalyDurationMsec) {

    thresholdAnomalyDurationMsec = anomalyDurationMsec;
}


template<typename Type, uint32_t MinTimeBetweenSamplesMsec, uint32_t SampleWindowMsec, uint32_t ThresholdWindowMsec, uint32_t SlopeWindowMsec>
void VariableMonitor<Type,MinTimeBetweenSamplesMsec,SampleWindowMsec, ThresholdWindowMsec,SlopeWindowMsec>::SetCheckBelowThreshold(bool checkBelowThresholdArg) {

    /// Setting whether we should check for values below or above the threshold.
    checkBelowThreshold = checkBelowThresholdArg;
}

template<typename Type, uint32_t MinTimeBetweenSamplesMsec, uint32_t SampleWindowMsec, uint32_t ThresholdWindowMsec, uint32_t SlopeWindowMsec>
bool VariableMonitor<Type,MinTimeBetweenSamplesMsec,SampleWindowMsec, ThresholdWindowMsec,SlopeWindowMsec>::CheckThreshold() {

    if (CheckAnomalyDuration(thresholdAnomalies, thresholdAnomalyDurationMsec)) {
        /// Clear the deque and return true if the first anomaly in the deque has lasted for at least thresholdAnomalyDurationMsec
        thresholdAnomalies.clear();
        return true;
    }
    return false;
}
*/

template<typename Type, uint32_t MinTimeBetweenSamplesMsec, uint32_t SampleWindowMsec, uint32_t ThresholdWindowMsec, uint32_t SlopeWindowMsec>
bool VariableMonitor<Type,MinTimeBetweenSamplesMsec,SampleWindowMsec, ThresholdWindowMsec,SlopeWindowMsec>::CheckIfValueConsistently(bool checkBelow, Type thresholdValue, uint32_t durationMsec, bool useCurrentTime, uint32_t minDataPoints) noexcept
{
	bool valuesAreConsistent = false;

	if( !values.empty())  // Early out for empty collection of timestamped values
	{
		iterator backIterator = values.rend();  // Should never be end element
		const uint32_t endTimeMsec = useCurrentTime ? GetElapsedTimeMsec() : backIterator->GetTimestamp();

		iterator endIterator = GetNewestEntry(endTimeMsec);
		if( endIterator != values.end() )
		{
			const uint32_t startTimeMsec = endTimeMsec - durationMsec;
			uint32_t countDataPointsInTimeSpan = 0;

			for( iterator valueIterator = GetOldestEntry(startTimeMsec);  valueIterator  != endIterator; ++valueIterator )  // Get oldest value in the collection window
			{
				 if ( (checkBelow && (valueIterator->GetValue() >= thresholdValue) ) || (valueIterator->GetValue() <= thresholdValue) )
				 {
					 return false;
				 }
				 ++countDataPointsInTimeSpan;
			}


			if (countDataPointsInTimeSpan >= minDataPoints)
			{
				valuesAreConsistent = true;

			}
		}
    }

    return valuesAreConsistent; /// All criteria are met.
}

template<typename Type, uint32_t MinTimeBetweenSamplesMsec, uint32_t SampleWindowMsec, uint32_t ThresholdWindowMsec, uint32_t SlopeWindowMsec>
bool VariableMonitor<Type,MinTimeBetweenSamplesMsec,SampleWindowMsec, ThresholdWindowMsec,SlopeWindowMsec>::CheckIfValueBetweenBoundConsistently(Type lowerThresholdValue, Type upperThresholdValue, uint32_t durationMsec, bool useCurrentTime, uint32_t minDataPoints) noexcept
{
    bool foundValuesInTimeSpan = false;

	if( !values.empty())  // Early out for empty collection of timestamped values
	{
		iterator backIterator = values.rend();  // Should never be end element
		const uint32_t endTimeMsec = useCurrentTime ? GetElapsedTimeMsec() : backIterator->GetTimestamp();

		iterator endIterator = GetNewestEntry(endTimeMsec);
		if( endIterator != values.end() )
		{
			const uint32_t startTimeMsec = endTimeMsec - durationMsec;
			uint32_t countDataPointsInTimeSpan = 0;


			for( iterator valueIterator = GetOldestEntry(startTimeMsec);  valueIterator  != endIterator; ++valueIterator )  // Get oldest value in the collection window
			{
				 if ( (valueIterator->GetValue() <= lowerThresholdValue)  || (valueIterator->GetValue() >= upperThresholdValue) )
				 {
					 return false;
				 }
				 ++countDataPointsInTimeSpan;
			}

			if (countDataPointsInTimeSpan >= minDataPoints)
			{
				foundValuesInTimeSpan = true;

			}
		}
	}

    return foundValuesInTimeSpan;  /// All criteria are met.
}

template<typename Type, uint32_t MinTimeBetweenSamplesMsec, uint32_t SampleWindowMsec, uint32_t ThresholdWindowMsec, uint32_t SlopeWindowMsec>
bool VariableMonitor<Type,MinTimeBetweenSamplesMsec,SampleWindowMsec, ThresholdWindowMsec,SlopeWindowMsec>::CheckIfValueOutOfBoundConsistently(Type lowerThresholdValue, Type upperThresholdValue, uint32_t durationMsec, bool useCurrentTime, uint32_t minDataPoints) noexcept
{
    /// No data to check.
	bool foundValuesInTimeSpan = false;

	if( !values.empty())  // Early out for empty collection of timestamped values
	{
		iterator backIterator = values.rend();  // Should never be end element
		const uint32_t endTimeMsec = useCurrentTime ? GetElapsedTimeMsec() : backIterator->GetTimestamp();

		iterator endIterator = GetNewestEntry(endTimeMsec);
		if( endIterator != values.end() )
		{
			const uint32_t startTimeMsec = endTimeMsec - durationMsec;
			uint32_t countDataPointsInTimeSpan = 0;


			for( iterator valueIterator = GetOldestEntry(startTimeMsec);  valueIterator  != endIterator; ++valueIterator )  // Get oldest value in the collection window
			{
				 if ( (valueIterator->GetValue() > lowerThresholdValue)  || (valueIterator->GetValue() < upperThresholdValue) )
				 {
					 return false;
				 }
				 ++countDataPointsInTimeSpan;
			}

			if (countDataPointsInTimeSpan >= minDataPoints)
			{
				foundValuesInTimeSpan = true;

			}
		}
	}

	return foundValuesInTimeSpan;  /// All criteria are met.
}

template<typename Type, uint32_t MinTimeBetweenSamplesMsec, uint32_t SampleWindowMsec, uint32_t ThresholdWindowMsec, uint32_t SlopeWindowMsec>
bool VariableMonitor<Type,MinTimeBetweenSamplesMsec,SampleWindowMsec, ThresholdWindowMsec,SlopeWindowMsec>::GetAverageValue(Type& averageValue, uint32_t durationMsec, bool useCurrentTime, uint32_t minDataPoints) const noexcept
{
	bool foundValuesInTimeSpan = false;

	if( !values.empty() )
	{
		if(minDataPoints > 0)   // Early out for empty collection of timestamped values or too few minimum points selected
		{
			volatile Type sum = 0;

			const_iterator lastIterator = values.crbegin();  // Last element
			const uint32_t endTimeMsec = useCurrentTime ? GetElapsedTimeMsec() : lastIterator->GetTimestamp();

			const_iterator endIterator = GetNewestEntry(endTimeMsec);
			if( endIterator != values.crend() )
			{
				volatile const uint32_t startTimeMsec = (endTimeMsec > durationMsec) ? (endTimeMsec - durationMsec) : 0 ;
				volatile uint32_t countDataPointsInTimeSpan = 0;

				const_iterator valueIterator = GetOldestEntry(startTimeMsec);

				if(valueIterator == endIterator  )  // COrner case of one entry in the list
				{
					countDataPointsInTimeSpan = 1;
					sum += valueIterator->GetValue();
				}
				else  // Sum the values between the two iterators (inclusively)
				{
					do
					{
						 sum += valueIterator->GetValue();
						 ++countDataPointsInTimeSpan;
					}
					while( valueIterator++ != endIterator);
				}


				if (countDataPointsInTimeSpan >= minDataPoints)
				{
					foundValuesInTimeSpan = true;

					 //// Calculate the average
					averageValue = static_cast<Type>(sum) / static_cast<Type>(countDataPointsInTimeSpan);

				}
			}
		}
	}
    return foundValuesInTimeSpan; //// All criteria are met.
}

template<typename Type, uint32_t MinTimeBetweenSamplesMsec, uint32_t SampleWindowMsec, uint32_t ThresholdWindowMsec, uint32_t SlopeWindowMsec>
bool VariableMonitor<Type,MinTimeBetweenSamplesMsec,SampleWindowMsec, ThresholdWindowMsec,SlopeWindowMsec>::GetAverageSchemeValue(Type& averageValue, AveragingScheme scheme, uint32_t durationMsec, bool useCurrentTime, uint32_t minDataPoints) noexcept {
	switch (scheme) {
		case AveragingScheme::MEAN:
			return GetAverageValue(averageValue, durationMsec, useCurrentTime, minDataPoints);
			break;
		case AveragingScheme::MEDIAN:
			break;
		case AveragingScheme::MODE:
			break;
		case AveragingScheme::GEOMETRIC:
			break;
		case AveragingScheme::HARMONIC:
			break;
		default:
			break;
	}
	return false;
}


template<typename Type, uint32_t MinTimeBetweenSamplesMsec, uint32_t SampleWindowMsec, uint32_t ThresholdWindowMsec, uint32_t SlopeWindowMsec>
bool VariableMonitor<Type,MinTimeBetweenSamplesMsec,SampleWindowMsec, ThresholdWindowMsec,SlopeWindowMsec>::CheckSlope() const
{

	/*

    /// Setting whether we should check for values below or above the threshold.
    if (CheckAnomalyDuration(slopeAnomalies, slopeAnomalyDurationMsec))
    {
        /// Clear the deque and return true if the first anomaly in the deque has lasted for at least slopeAnomalyDurationMsec
        slopeAnomalies.clear();
        return true;
    }
    */
    return false;
}

template<typename Type, uint32_t MinTimeBetweenSamplesMsec, uint32_t SampleWindowMsec, uint32_t ThresholdWindowMsec, uint32_t SlopeWindowMsec>
bool VariableMonitor<Type,MinTimeBetweenSamplesMsec,SampleWindowMsec, ThresholdWindowMsec,SlopeWindowMsec>::CheckIfSlope(bool checkBelow, double slopeThresh, bool useAbsolute, uint32_t deltaTimeMsec, bool useCurrentTime) const
{

	return false;
	/*
    double calculatedSlope;
    if (!GetSimpleSlopeOverDeltaTime(deltaTimeMsec, calculatedSlope, useCurrentTime)) {
        return false;  /// Returning false because slope calculation was not successful
    }

    /// Optionally use the absolute value of the calculated slope and threshold.
    if (useAbsolute) {
        calculatedSlope = fabs(calculatedSlope);
        slopeThresh = fabs(slopeThresh);
    }

    /// Check the slope based on the direction specified.
    if (checkBelow) {
        /// Check if the slope is above the threshold.
        return calculatedSlope > slopeThresh;
    } else {
        /// Check if the slope is below the threshold.
        return calculatedSlope < slopeThresh;
    }
    */
}



/*
template<typename Type, uint32_t MinTimeBetweenSamplesMsec, uint32_t SampleWindowMsec, uint32_t ThresholdWindowMsec, uint32_t SlopeWindowMsec>
uint32_t VariableMonitor<Type,MinTimeBetweenSamplesMsec,SampleWindowMsec, ThresholdWindowMsec,SlopeWindowMsec>::GetLastSlopeAnomalyTimeMsec() const {
    return lastSlopeAnomalyTimeMsec;
}

template<typename Type, uint32_t MinTimeBetweenSamplesMsec, uint32_t SampleWindowMsec, uint32_t ThresholdWindowMsec, uint32_t SlopeWindowMsec>
uint32_t VariableMonitor<Type,MinTimeBetweenSamplesMsec,SampleWindowMsec, ThresholdWindowMsec,SlopeWindowMsec>::GetLastThresholdAnomalyTime() const {
    return lastThresholdAnomalyTimeMsec;
}

*/
template<typename Type, uint32_t MinTimeBetweenSamplesMsec, uint32_t SampleWindowMsec, uint32_t ThresholdWindowMsec, uint32_t SlopeWindowMsec>
bool VariableMonitor<Type,MinTimeBetweenSamplesMsec,SampleWindowMsec, ThresholdWindowMsec,SlopeWindowMsec>::GetSimpleSlopeOverDeltaTime(uint32_t deltaTimeMsec, double &calculatedSlope, bool useCurrentTime) noexcept
{
	bool foundValuesInTimeSpan = false;
	if( !values.empty() )  // Early out for empty collection of timestamped values or too few minimum points selected
	{
		iterator backIterator = values.end();  // Should never be end element
		const uint32_t endTimeMsec = useCurrentTime ? GetElapsedTimeMsec() : backIterator->GetTimestamp();

		iterator endIterator = GetNewestEntry(endTimeMsec);
		if( endIterator != values.end() )
		{
			const uint32_t startTimeMsec = endTimeMsec - deltaTimeMsec;

			iterator startIterator = GetOldestEntry(startTimeMsec);
			if( startIterator != values.end() )
			{
			    if( startIterator->GetTimestamp() != endIterator->GetTimestamp() )
			    {
			    	foundValuesInTimeSpan = true;

			    	/// Compute the slope using the formula:
			    	/// Slope = (Y2 - Y1) / (X2 - X1) where X represents time and Y represents value.
			    	calculatedSlope = static_cast<double>(endIterator->GetValue() - startIterator->GetValue() ) /
			    			          static_cast<double>(endIterator->GetTimestamp() - startIterator->GetTimestamp() );
			    }
			}
		}
	}
	return foundValuesInTimeSpan; //// All criteria are met.
}

template<typename Type, uint32_t MinTimeBetweenSamplesMsec, uint32_t SampleWindowMsec, uint32_t ThresholdWindowMsec, uint32_t SlopeWindowMsec>
bool VariableMonitor<Type,MinTimeBetweenSamplesMsec,SampleWindowMsec, ThresholdWindowMsec,SlopeWindowMsec>::GetAdvancedSlopeOverDeltaTime(uint32_t deltaTimeMsec, double &resultSlope, SlopeCalculationType calcType, uint32_t windowSize) noexcept
{
	UTIL_UNUSED(deltaTimeMsec);
	UTIL_UNUSED(resultSlope);
	UTIL_UNUSED(calcType);
	UTIL_UNUSED(windowSize);

    /// Check if there's any data to compute from or if the provided window size is invalid.

    bool foundValuesInTimeSpan = false;
    if( !values.empty()  && ( windowSize >= 2) ) // Early out for empty collection of timestamped values or too few minimum points selected
    {


    	/*

    /// Calculate the start time of our data window based on the provided delta time.
    uint32_t currentTime = GetElapsedTimeMsec();
    uint32_t startTime = currentTime - deltaTimeMsec;

    /// Set an iterator to the end of our data list.
    auto it = values.end();

    /// Move the iterator backwards to find the start point of our data within the required time range.
    while (it != values.begin()) {
        --it;
        if (it->second <= startTime) {
            break;
        }
    }

    /// Check if we have enough data points to compute the slope.
    if ((it == values.begin() && it->second > startTime) || static_cast<uint32_t>(std::distance(it, values.end())) < windowSize) {
        return false;
    }

    /// Create a moving window of data points starting from our iterator.
    std::deque<std::pair<Type, uint32_t>> window(it, it + windowSize);

    /// Calculate the average of the data values within the window.
    double previousAverage = std::accumulate(window.begin(), window.end(), 0.0,
        [](double acc, const std::pair<Type, uint32_t>& p) { return acc + p.first; }) / windowSize;

    /// Vector to store the slopes calculated
    std::vector<double> slopes;

    /// Slide the window over the data and recalculate the slope with each move.
    for (auto jt = it + windowSize; jt != values.end(); ++jt) {
        window.pop_front();
        window.push_back(*jt);

        /// Calculate the new average of the data values within the moved window.
        double currentAverage = std::accumulate(window.begin(), window.end(), 0.0,
            [](double acc, const std::pair<T, uint32_t>& p) { return acc + p.first; }) / windowSize;

        /// Compute the slope using the difference between the new and previous averages over the window's time range.
        double currentSlope = (currentAverage - previousAverage) / (window.back().second - window.front().second);

        /// Store calculated slope
        slopes.push_back(currentSlope);

        /// Update the previous average for the next iteration.
        previousAverage = currentAverage;
    }

    return CalculateSlopeFromType(slopes, calcType, resultSlope);
    */

    }
    return foundValuesInTimeSpan;
}

template<typename Type, uint32_t MinTimeBetweenSamplesMsec, uint32_t SampleWindowMsec, uint32_t ThresholdWindowMsec, uint32_t SlopeWindowMsec>
void VariableMonitor<Type,MinTimeBetweenSamplesMsec,SampleWindowMsec, ThresholdWindowMsec,SlopeWindowMsec>::Erase() noexcept
{
	values.Erase();
	thresholdAnomalies.Erase();
	slopeAnomalies.Erase();
}


/*

//==============================================================//
/// HELPERS
//==============================================================//
template<typename Type, uint32_t MinTimeBetweenSamplesMsec, uint32_t SampleWindowMsec, uint32_t ThresholdWindowMsec, uint32_t SlopeWindowMsec>
bool VariableMonitor<Type,MinTimeBetweenSamplesMsec,SampleWindowMsec, ThresholdWindowMsec,SlopeWindowMsec>::CheckAnomalyDuration(const std::deque<uint32_t>& anomaliesDeque, uint32_t anomalyDuration) {
    if (!anomaliesDeque.empty() && ((GetElapsedTimeMsec() - anomaliesDeque.front()) >= anomalyDuration)) {
        return true;
    }
    return false;
}


template<typename Type, uint32_t MinTimeBetweenSamplesMsec, uint32_t SampleWindowMsec, uint32_t ThresholdWindowMsec, uint32_t SlopeWindowMsec>
bool VariableMonitor<Type,MinTimeBetweenSamplesMsec,SampleWindowMsec, ThresholdWindowMsec,SlopeWindowMsec>::CalculateSlopeFromType(const std::vector<double> &slopes, SlopeCalculationType calcType, double &resultSlope) {
    if (slopes.empty()) {
        /// Handle empty slope vector.
        return false;
    }

    switch (calcType) {
        case SlopeCalculationType::AVERAGE:
        {
            resultSlope = std::accumulate(slopes.begin(), slopes.end(), 0.0) / slopes.size();
            return true;
        }

        case SlopeCalculationType::MAXIMUM:
        {
            resultSlope = *std::max_element(slopes.begin(), slopes.end());
            return true;
        }

        case SlopeCalculationType::MINIMUM:
        {
            resultSlope = *std::min_element(slopes.begin(), slopes.end());
            return true;
        }

        case SlopeCalculationType::CHANGE:
        {
            double maxDifference = 0.0;
            for (size_t i = 1; i < slopes.size(); ++i) {
                double diff = fabs(slopes[i] - slopes[i-1]);
                if (diff > maxDifference) {
                    maxDifference = diff;
                }
            }
            resultSlope = maxDifference;
            return true;
        }

        case SlopeCalculationType::LAST:
        {
            resultSlope = slopes.back();
            return true;
        }

        default:
        {
            /// Handle unknown calculation type.
            return false;
        }
    }
}

*/





#endif // ValueMonitor_H
