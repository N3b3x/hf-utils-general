/**
 * @file VariableAnomalyMonitor.h
 * @brief Monitors variables for anomalies based on slope and threshold criteria.
 *
 * This module provides functionality to detect anomalies in a sequence of values.
 * Anomalies can be of two types:
 * 1. Slope Anomalies: Detected based on the rate of change of values over a specified time window.
 * 2. Threshold Anomalies: Detected when values breach a predefined threshold.
 *
 * The monitoring system keeps track of the most recent anomalies and clears historical data
 * once non-anomalous values are received. This ensures focus on the most recent anomalies and reduces memory usage.
 *
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


#ifndef VariableAnomalyMonitor_H
#define VariableAnomalyMonitor_H

#include <deque>
#include <utility>
#include <cstdint> // for uint32_t

#include "UTILITIES/common/VariableTrackerBase.h"
#include "UTILITIES/common/Mutex.h"

#define SET_CHECK_BELOW_THRESHOLD   true
#define SET_CHECK_ABOVE_THRESHOLD   false

#define SET_ABSOLUTE_SLOPE_CHECKING     true
#define SET_DIRECTIONAL_SLOPE_CHECKING  false

/**
 * @brief A class to monitor a continuously updated variable and notify
 * when the slope exceeds a limit or the value crosses a threshold.
 */
template<typename T>
class VariableAnomalyMonitor final : public VariableTrackerBase<T> {
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
    VariableAnomalyMonitor(uint32_t minTimeBetweenSampleStoreMsec,
                           T threshold,
                           uint32_t thresholdTimePeriodMsec,
                           uint32_t thresholdAnomalyDurationMsec,
                           bool checkBelowThreshold = true,
                           double slopeLimit = 0.0,
                           uint32_t slopeTimePeriodMsec = 0,
                           uint32_t slopeAnomalyDurationMsec = 0,
                           bool checkBelowSlopeThreshold = false,
                           bool useAbsoluteSlope = true);

    /**
     * @brief Update the variable with a new value.
     * This should be called every time the variable being monitored changes.
     *
     * @param newValue The new value of the variable.
     *
     * @return True if the data was actually stored in tracker.
     * @return False if the data was not stored due to violating minTimeBetweenSampleStoreMsec.
     */
    bool UpdateValue(T newValue);

    /**
     * @brief Gets the most recent value stored in the deque.
     *
     * @return T The most recent value in the deque.
     */
    T GetLastValue() const ;

    /**
	 * @brief Gets the maximum value in the deque.
	 * @param value  The largest value in the deque.
	 * @return true if value available, false otherwise.
	 */
    bool GetMaxValue( T& value ) const;

    /**
	  * @brief Gets the minimum value in the deque.
	  * @param value  The smallest value in the deque.
	  *  @return true if value available, false otherwise.
	  */
     bool GetMinValue( T& value ) const;

    /**
     * @brief Sets the minimum time difference from last data to store new data.
     * @param minTimeBetweenUpdateMsec Minimum time difference in milliseconds.
     */
    void SetMinTimeBetweenUpdateStoreMsec(uint32_t minTimeBetweenUpdateMsec);

    /**
     * @brief Set the limit for the variable's rate of change (slope) and
     * the time period over which to calculate it.
     *
     * @param slope The slope limit. The function checkSlope() will return true
     * if the variable's rate of change exceeds this limit.
     * @param timePeriodMsec The time period (in milliseconds) over which to calculate the slope.
     * @param anomalyDurationMsec The duration of time (in milliseconds) for which the anomaly must exist.
     */
    void SetSlopeLimit(double slope, uint32_t timePeriodMsec, uint32_t anomalyDurationMsec);

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
    void UseAbsoluteSlope(bool useAbsolute);

    /**
     * @brief Set the threshold, the time period for threshold checking,
     * and the duration of time for which the anomaly must exist.
     *
     * @param threshold The threshold value. The function checkThreshold()
     *                  will return true if the variable's value is consistently on one side of this threshold for the set time period.
     */
    void SetThreshold(T threshold);

    /**
     * @brief Set the threshold duration of time for which the anomaly must exist.
     *
     * @param anomalyDurationMsec The duration of time (in milliseconds) for which the anomaly must exist.
     */
    void SetThresholdAnomalyDurationMsec(uint32_t anomalyDurationMsec);

    /**
     * @brief Set the threshold, the time period for threshold checking,
     * and the duration of time for which the anomaly must exist.
     *
     * @param threshold The threshold value. The function checkThreshold()
     * will return true if the variable's value is consistently on one side of this threshold for the set time period.
     * @param timePeriodMsec The time period (in milliseconds) over which to check if the variable is below or above the threshold.
     * @param anomalyDurationMsec The duration of time (in milliseconds) for which the anomaly must exist.
     */
    void SetThreshold(T threshold, uint32_t timePeriodMsec, uint32_t anomalyDurationMsec);

    /**
     * @brief Set the Check Below Threshold flag
     *
     * @param checkBelowThresholdArg Specifies whether to check for values below or above the threshold.
     */
    void SetCheckBelowThreshold(bool checkBelowThresholdArg); // New member function

    /**
     * @brief Check if the variable has been consistently below or above
     * the threshold for the set time period.
     *
     * @return true If the variable has been below or above the threshold for the set time period
     * @return false Otherwise
     */
    bool CheckThreshold();

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
    bool CheckIfValueConsistently(bool checkBelow, T thresholdValue, uint32_t durationMsec, bool useCurrentTime = true, uint32_t minDataPoints = 2) noexcept override;

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
    bool CheckIfValueBetweenBoundConsistently(T lowerThresholdValue, T upperThresholdValue, uint32_t durationMsec, bool useCurrentTime, uint32_t minDataPoints = 2) noexcept override;

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
    bool CheckIfValueOutOfBoundConsistently(T lowerThresholdValue, T upperThresholdValue, uint32_t durationMsec, bool useCurrentTime, uint32_t minDataPoints = 2) noexcept override;

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
    bool GetAverageValue(T& averageValue, uint32_t durationMsec, bool useCurrentTime, uint32_t minDataPoints = 2) noexcept;

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
    bool GetAverageSchemeValue(T& averageValue, AveragingScheme scheme, uint32_t durationMsec, bool useCurrentTime, uint32_t minDataPoints = 2) noexcept override;

    /**
     * @brief Check if the variable's rate of change (slope) has exceeded
     * the limit within the set time period.
     *
     * @return true If the slope is greater than the limit
     * @return false Otherwise
     */
    bool CheckSlope();

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
    bool CheckIfSlope(bool checkBelow, double slopeThresh, bool useAbsolute, uint32_t deltaTimeMsec, bool useCurrentTime = true);

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
    uint32_t GetLastSlopeAnomalyTimeMsec() const;

    /**
     * @brief Retrieves the most recent time when a threshold anomaly was detected.
     *
     * @return uint32_t The timestamp of the most recent threshold anomaly.
     */
    uint32_t GetLastThresholdAnomalyTime() const;

    /**
     * @brief Removes old values and anomalies from the deques.
     */
    void Cleanup();

    /**
     * @brief Removes all values and anomalies from the deques.
     */
    void CleanupAll();

    uint32_t GetValueCount() noexcept
    {
    	return values.size();
    }

private:

    /**
     * @brief Checks if the first anomaly in the deque has persisted for a specified duration.
     *
     * This function examines the front of the provided anomalies deque to see if the time
     * elapsed since the first anomaly exceeds or matches the provided anomaly duration.
     *
     * @param anomaliesDeque A deque containing timestamps of detected anomalies.
     * @param anomalyDuration The duration (in milliseconds) to check the anomaly persistence against.
     * @return Returns true if the first anomaly in the deque has persisted for at least the specified duration, false otherwise.
     */
    bool CheckAnomalyDuration(const std::deque<uint32_t>& anomaliesDeque, uint32_t anomalyDuration);

    /**
     * @brief Cleans up a deque of pairs (value and timestamp) based on a specified time limit.
     *
     * @tparam T The data type of the values stored in the deque.
     * @param[in, out] deque The deque of pairs to be cleaned.
     * @param[in] currentTime The current timestamp.
     * @param[in] timeLimit The maximum age of data to retain in the deque.
     */
    void CleanDeque(std::deque<std::pair<T, uint32_t>>& deque, uint32_t currentTime, uint32_t timeLimit);

    /**
     * @brief Cleans up a deque of timestamps based on a specified time limit.
     *
     * @param[in, out] deque The deque of timestamps to be cleaned.
     * @param[in] currentTime The current timestamp.
     * @param[in] timeLimit The maximum age of data to retain in the deque.
     */
    void CleanDeque(std::deque<uint32_t>& deque, uint32_t currentTime, uint32_t timeLimit);

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

    std::deque<std::pair<T, uint32_t>> values;      ///< Deque to store value and timestamp pairs.
    uint32_t minTimeBetweenUpdateSampleStoringMsec; ///< Minimum timestamp difference the new data must have before allowing storage in Deque.
                                                    ///< Allows user to set some bounds on the amount of data stored in deque.

    Mutex mutex;          						///< ThreadX mutex object for thread safety
    Mutex configMutex;          				///< ThreadX mutex object for thread safety
    static const char mutexName[];				///< ThreadX mutex object name
    static const char configMutexName[];		///< ThreadX mutex object name

    T threshold;                                ///< The threshold value.
    bool checkBelowThreshold;                   ///< True if checking for values below the threshold, false if checking for values above the threshold.
    uint32_t thresholdTimePeriodMsec;           ///< The time period for threshold checking.
    uint32_t thresholdAnomalyDurationMsec;      ///< The duration of time for which the anomaly must exist.
    uint32_t lastThresholdAnomalyTimeMsec;      ///< Holds the most recent time when a threshold anomaly was detected.
    std::deque<uint32_t> thresholdAnomalies;    ///< Deque to store the starting times of threshold anomalies.

    double slopeLimit;                          ///< The slope limit.
    bool useAbsoluteSlope;                      ///< When true, helps detect anomalies when the variable is changing too fast, regardless of direction.
    bool checkBelowSlopeLim;                    ///< True if checking for slope values below the limit, false if checking for values above the limit.
    uint32_t slopeTimePeriodMsec;               ///< The time period for slope calculation.
    uint32_t slopeAnomalyDurationMsec;          ///< The time period the slope anomaly must last to be considered.
    uint32_t lastSlopeAnomalyTimeMsec;          ///< Holds the most recent time when a slope anomaly was detected.
    std::deque<uint32_t> slopeAnomalies;        ///< Deque to store the starting times of slope anomalies.
};

#endif // THRESHOLD_NOTIFIER_H
