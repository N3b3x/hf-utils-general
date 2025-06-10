/**
 * @file VariableAnomalyMonitor.cpp
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

#include "math.h"
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <map>
#include <cmath>

#include "VariableTrackerBase.h"
#include "VariableAnomalyMonitor.h"

/// Explicit instantiations:
template class VariableAnomalyMonitor<float>;
template class VariableAnomalyMonitor<int>;

template<typename T>
const char VariableAnomalyMonitor<T>::mutexName[] = "VariableAnomalyMonitorClassMutex";

template<typename T>
const char VariableAnomalyMonitor<T>::configMutexName[] = "VariableAnomalyMonitorClassConfigMutex";

static constexpr uint32_t MinTimeBetweenSamples = 1U;

template<typename T>
VariableAnomalyMonitor<T>::VariableAnomalyMonitor(uint32_t minTimeBetweenSampleStoreMsec,
        T thresholdArg,
        uint32_t thresholdTimePeriodMsecArg,
        uint32_t thresholdAnomalyDurationMsecArg,
        bool checkBelowThresholdArg,
        double slopeLimitArg,
        uint32_t slopeTimePeriodMsecArg,
        uint32_t slopeAnomalyDurationMsecArg,
        bool checkBelowSlopeThresholdArg,
        bool useAbsoluteSlopeArg)
 :
    values(),
    minTimeBetweenUpdateSampleStoringMsec( std::max( minTimeBetweenSampleStoreMsec, MinTimeBetweenSamples)),   // Must be at least 1 msec to avoid zero divide on slope.
	mutex(mutexName),
	configMutex(configMutexName),
    threshold(thresholdArg),
    checkBelowThreshold(checkBelowThresholdArg),
    thresholdTimePeriodMsec(thresholdTimePeriodMsecArg),
    thresholdAnomalyDurationMsec(thresholdAnomalyDurationMsecArg),
    lastThresholdAnomalyTimeMsec(0),
    thresholdAnomalies(),
    slopeLimit(slopeLimitArg),
    useAbsoluteSlope(useAbsoluteSlopeArg),
    checkBelowSlopeLim(checkBelowSlopeThresholdArg),
    slopeTimePeriodMsec(slopeTimePeriodMsecArg),
    slopeAnomalyDurationMsec(slopeAnomalyDurationMsecArg),
    lastSlopeAnomalyTimeMsec(0),
    slopeAnomalies()
{
    /// The constructor initializing all values. No logic is implemented here.
}

template<typename T>
bool VariableAnomalyMonitor<T>::UpdateValue(T newValue) {
    uint32_t currentTime = GetElapsedTimeMsec();

    /// TODO: Consider higher resolution timer if updates can occur within the same millisecond.

    /// Check if the update is too frequent based on the last stored timestamp in the deque
    if (!values.empty() && ((currentTime - values.back().second) < minTimeBetweenUpdateSampleStoringMsec)) {
        /// If so, skip the update
        return false;
    }

    /// Add the new value to the deque. But first we need to ensure that we won't get a zero divide in the slope.
    /// Check if the deque is not empty and the current time is the same as the back's timestamp. It shouldn't but doesn't hurt
    if (!values.empty() && values.back().second == currentTime) {
        /// Update the value of the back entry.
        values.back().first = newValue;
    } else {
        /// Add the new value to the deque.
        values.push_back(std::make_pair(newValue, currentTime));
    }

    /// Check if there's a slope anomaly, from the first and the last value in the time window range we're allowed to look into.
    /// This is done by calculating the slope between oldest and newest data in the time window and comparing it to the slope limit.



    double calculatedSlope = (values.back().first - values.front().first) / static_cast<double>(values.back().second - values.front().second);

    if (useAbsoluteSlope) {
        /// Use absolute values for comparison
        if ((fabs(calculatedSlope) > fabs(slopeLimit)) == !checkBelowSlopeLim) {
            /// Anomaly detected
            slopeAnomalies.push_back(currentTime);
            lastSlopeAnomalyTimeMsec = currentTime;
        } else {
            /// No anomaly
            slopeAnomalies.clear();
        }
    } else {
        /// Use actual values for comparison
        if ((calculatedSlope > slopeLimit) == !checkBelowSlopeLim) {
            /// Anomaly detected
            slopeAnomalies.push_back(currentTime);
            lastSlopeAnomalyTimeMsec = currentTime;
        } else {
            /// No anomaly
            slopeAnomalies.clear();
        }
    }


    /// Check if there's a threshold anomaly. (We must first check if what we're looking into is a value above
    /// or below threshold. We use the ternary operator to decide which side of the threshold we should check.
    if ((checkBelowThreshold ? newValue < threshold : newValue > threshold)) {
        /// If there is, add the current time to the thresholdAnomalies deque.
        thresholdAnomalies.push_back(currentTime);
        lastThresholdAnomalyTimeMsec = currentTime;
    }
    else {
        /// If not anomaly, clear the thresholdAnomalies deque.
        thresholdAnomalies.clear();
    }

    /// Remove old values and anomalies that are outside the time period we're interested in.
    Cleanup();

    return true;
}

template<typename T>
T VariableAnomalyMonitor<T>::GetLastValue() const {
    if (!values.empty()) {
        return values.back().first;
    } else {
        /// Handle the case when the deque is empty.
        return T(); /// Default value for T
    }
}

template<typename T> bool VariableAnomalyMonitor<T>::GetMaxValue( T& value ) const
{
	if (!values.empty())
	{
		auto iterator = values.begin();
		value = (*iterator).first;
		   ++iterator;
		while(iterator != values.end())
		{
			if(value <  (*iterator).first)
			{
				value = (*iterator).first;
			}
			++iterator;
		}

		return true;
	}
	else
	{
	    return false;
	}
}

template<typename T> bool VariableAnomalyMonitor<T>::GetMinValue( T& value ) const
{
	if (!values.empty())
	{
		auto iterator = values.begin();
		value = (*iterator).first;
		   ++iterator;
		while(iterator != values.end())
		{
			if( value >  (*iterator).first)
			{
				value = (*iterator).first;
			}
			++iterator;
		}

		return true;
	}
	else
	{
		return false;
	}
}


template<typename T>
void VariableAnomalyMonitor<T>::SetMinTimeBetweenUpdateStoreMsec(uint32_t minTimeBetweenUpdateMsec) {
    minTimeBetweenUpdateSampleStoringMsec = minTimeBetweenUpdateMsec;
}

template<typename T>
void VariableAnomalyMonitor<T>::SetSlopeLimit(double slope, uint32_t timePeriodMsec, uint32_t anomalyDurationMsec) {
	MutexGuard guard(configMutex);

    /// Setting the slope limit, time period for slope calculation and duration of time for which the slope anomaly must exist.
    slopeLimit = slope;
    slopeTimePeriodMsec = timePeriodMsec;
    slopeAnomalyDurationMsec = anomalyDurationMsec;
}

template<typename T>
void VariableAnomalyMonitor<T>::UseAbsoluteSlope(bool useAbsolute) {
	MutexGuard guard(configMutex);
    useAbsoluteSlope = useAbsolute;
}

template<typename T>
void VariableAnomalyMonitor<T>::SetThreshold(T newThreshold, uint32_t timePeriodMsec, uint32_t anomalyDurationMsec) {
	MutexGuard guard(configMutex);

    /// Setting the threshold, time period for threshold checking and duration of time for which the threshold anomaly must exist.
    threshold = newThreshold;
    thresholdAnomalyDurationMsec = anomalyDurationMsec;
    thresholdTimePeriodMsec = timePeriodMsec;
}

template<typename T>
void VariableAnomalyMonitor<T>::SetThreshold(T newThreshold) {
	MutexGuard guard(configMutex);

    /// Setting the threshold, time period for threshold.
    threshold = newThreshold;
}

template<typename T>
void VariableAnomalyMonitor<T>::SetThresholdAnomalyDurationMsec(uint32_t anomalyDurationMsec) {
	MutexGuard guard(configMutex);

    thresholdAnomalyDurationMsec = anomalyDurationMsec;
}

template<typename T>
void VariableAnomalyMonitor<T>::SetCheckBelowThreshold(bool checkBelowThresholdArg) {
	MutexGuard guard(configMutex);

    /// Setting whether we should check for values below or above the threshold.
    checkBelowThreshold = checkBelowThresholdArg;
}

template<typename T>
bool VariableAnomalyMonitor<T>::CheckThreshold() {
    if (CheckAnomalyDuration(thresholdAnomalies, thresholdAnomalyDurationMsec)) {
        /// Clear the deque and return true if the first anomaly in the deque has lasted for at least thresholdAnomalyDurationMsec
        thresholdAnomalies.clear();
        return true;
    }
    return false;
}

template<typename T>
bool VariableAnomalyMonitor<T>::CheckIfValueConsistently(bool checkBelow, T thresholdValue, uint32_t durationMsec, bool useCurrentTime, uint32_t minDataPoints) noexcept {
    /// Step 1: No data to check.
    if (values.empty()) {
        return false;
    }

    /// Step 2: Calculate start and end times.
    uint32_t endTime = useCurrentTime ? GetElapsedTimeMsec() : values.back().second;
    uint32_t startTime = endTime - durationMsec;

    /// Step 3: Check if available data spans the required duration.
    if (values.back().second - values.front().second < durationMsec) {
        return false;
    }

    bool foundValuesInTimeSpan = false;
    uint32_t countDataPointsInTimeSpan = 0;

    /// Step 4: Iterate through the values
    for (auto it = values.rbegin(); it != values.rend(); ++it) {
        T value = it->first;
        uint32_t timestamp = it->second;

        if (timestamp < startTime) {
            break; /// All relevant values have been checked.
        }

        foundValuesInTimeSpan = true;
        countDataPointsInTimeSpan++;

        /// Check value against threshold.
        if (checkBelow ? (value >= thresholdValue) : (value <= thresholdValue)) {
            return false;
        }
    }

    /// Step 5: Check if enough data points were examined.
    if (countDataPointsInTimeSpan < minDataPoints) {
        return false;
    }

    /// Step 6: Return true if all criteria are met.
    return foundValuesInTimeSpan;
}

template<typename T>
bool VariableAnomalyMonitor<T>::CheckIfValueBetweenBoundConsistently(T lowerThresholdValue, T upperThresholdValue, uint32_t durationMsec, bool useCurrentTime, uint32_t minDataPoints) noexcept {
    /// No data to check.
    if (values.empty()) {
        return false;
    }

    /// Calculate start and end times.
    uint32_t endTime = useCurrentTime ? GetElapsedTimeMsec() : values.back().second;
    uint32_t startTime = endTime - durationMsec;

    /// Check if available data spans the required duration.
    if (values.back().second - values.front().second < durationMsec) {
        return false;
    }

    bool foundValuesInTimeSpan = false;
    uint32_t countDataPointsInTimeSpan = 0;

    /// Iterate through the values
    for (auto it = values.rbegin(); it != values.rend(); ++it) {
        T value = it->first;
        uint32_t timestamp = it->second;

        if (timestamp < startTime) {
            break; /// All relevant values have been checked.
        }

        foundValuesInTimeSpan = true;
        countDataPointsInTimeSpan++;

        /// Check value against lower and upper thresholds.
        if (value <= lowerThresholdValue || value >= upperThresholdValue) {
            return false; /// Value outside of bounds.
        }
    }

    /// Check if enough data points were examined.
    if (countDataPointsInTimeSpan < minDataPoints) {
        return false;
    }

    /// All criteria are met.
    return foundValuesInTimeSpan;
}

template<typename T>
bool VariableAnomalyMonitor<T>::CheckIfValueOutOfBoundConsistently(T lowerThresholdValue, T upperThresholdValue, uint32_t durationMsec, bool useCurrentTime, uint32_t minDataPoints) noexcept {
    /// No data to check.
    if (values.empty()) {
        return false;
    }

    /// Calculate start and end times.
    uint32_t endTime = useCurrentTime ? GetElapsedTimeMsec() : values.back().second;
    uint32_t startTime = endTime - durationMsec;

    /// Check if available data spans the required duration.
    if (values.back().second - values.front().second < durationMsec) {
        return false;
    }

    bool foundValuesInTimeSpan = false;
    uint32_t countDataPointsInTimeSpan = 0;

    /// Iterate through the values
    for (auto it = values.rbegin(); it != values.rend(); ++it) {
        T value = it->first;
        uint32_t timestamp = it->second;

        if (timestamp < startTime) {
            break; /// All relevant values have been checked.
        }

        foundValuesInTimeSpan = true;
        countDataPointsInTimeSpan++;

        /// Check value against lower and upper thresholds.
        if (value >= lowerThresholdValue && value <= upperThresholdValue) {
            return false; /// Value inside of bounds.
        }
    }

    /// Check if enough data points were examined.
    if (countDataPointsInTimeSpan < minDataPoints) {
        return false;
    }

    /// All criteria are met.
    return foundValuesInTimeSpan;
}

template<typename T>
bool VariableAnomalyMonitor<T>::GetAverageValue(T& averageValue, uint32_t durationMsec, bool useCurrentTime, uint32_t minDataPoints) noexcept {
    //// No data to check.
    if (values.empty()) {
        return false;
    }

    //// Calculate start and end times.
    uint32_t endTime = useCurrentTime ? GetElapsedTimeMsec() : values.back().second;
    uint32_t startTime = endTime - durationMsec;

    //// Check if available data spans the required duration.
    if (values.back().second - values.front().second < durationMsec) {
        return false;
    }

    bool foundValuesInTimeSpan = false;
    uint32_t countDataPointsInTimeSpan = 0;
    T sum = 0;

    //// Iterate through the values
    for (auto it = values.rbegin(); it != values.rend(); ++it) {
        T value = it->first;
        uint32_t timestamp = it->second;

        if (timestamp < startTime) {
            break; //// All relevant values have been checked.
        }

        foundValuesInTimeSpan = true;
        countDataPointsInTimeSpan++;
        sum += value;
    }

    //// Check if enough data points were examined.
    if (countDataPointsInTimeSpan < minDataPoints) {
        return false;
    }

    //// Calculate the average
    averageValue = static_cast<T>(sum) / static_cast<T>(countDataPointsInTimeSpan);

    //// All criteria are met.
    return foundValuesInTimeSpan;
}

template<typename T>
bool VariableAnomalyMonitor<T>::GetAverageSchemeValue(T& averageValue, AveragingScheme scheme, uint32_t durationMsec, bool useCurrentTime, uint32_t minDataPoints) noexcept {
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


template<typename T>
bool VariableAnomalyMonitor<T>::CheckSlope() {
    /// Setting whether we should check for values below or above the threshold.
    if (CheckAnomalyDuration(slopeAnomalies, slopeAnomalyDurationMsec)) {
        /// Clear the deque and return true if the first anomaly in the deque has lasted for at least slopeAnomalyDurationMsec
        slopeAnomalies.clear();
        return true;
    }
    return false;
}

template<typename T>
bool VariableAnomalyMonitor<T>::CheckIfSlope(bool checkBelow, double slopeThresh, bool useAbsolute, uint32_t deltaTimeMsec, bool useCurrentTime) {
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
}


template<typename T>
uint32_t VariableAnomalyMonitor<T>::GetLastSlopeAnomalyTimeMsec() const {
    return lastSlopeAnomalyTimeMsec;
}

template<typename T>
uint32_t VariableAnomalyMonitor<T>::GetLastThresholdAnomalyTime() const {
    return lastThresholdAnomalyTimeMsec;
}

template<typename T>
void VariableAnomalyMonitor<T>::Cleanup() {
    uint32_t currentTime = GetElapsedTimeMsec();

    /// Clean values deque
    CleanDeque(values, currentTime, std::max(slopeTimePeriodMsec, thresholdTimePeriodMsec));

    /// Clean slopeAnomalies deque
    CleanDeque(slopeAnomalies, currentTime, slopeAnomalyDurationMsec);

    /// Clean thresholdAnomalies deque
    CleanDeque(thresholdAnomalies, currentTime, thresholdAnomalyDurationMsec);
}

template<typename T>
void VariableAnomalyMonitor<T>::CleanupAll() {
    uint32_t currentTime = GetElapsedTimeMsec();

    /// Clean values deque
    CleanDeque(values, currentTime, 0);

    /// Clean slopeAnomalies deque
    CleanDeque(slopeAnomalies, currentTime, 0);

    /// Clean thresholdAnomalies deque
    CleanDeque(thresholdAnomalies, currentTime, 0);
}


template<typename T>
bool VariableAnomalyMonitor<T>::GetSimpleSlopeOverDeltaTime(uint32_t deltaTimeMsec, double &calculatedSlope, bool useCurrentTime) noexcept {
    /// Check if the values deque is empty. If it is, there's no data to compute the slope from.
    if (values.empty()) {
        return false;
    }

    /// Decide the end time based on the useCurrentTime flag
    uint32_t endTime = useCurrentTime ? GetElapsedTimeMsec() : values.back().second;

    /// Calculate the start time from which data points will be considered based on the provided delta time.
    uint32_t startTime = endTime - deltaTimeMsec;

    /// Create an iterator to find the oldest data point within the specified time range.
    auto oldest = values.begin();
    /// Iterate over data points until a data point within the required time range is found.
    while (oldest != values.end() && oldest->second < startTime) {
        ++oldest;
    }

    /// Check if there are enough data points to calculate the slope.
    if (oldest == values.end() || oldest->second == endTime) {
        return false;
    }

    /// Compute the slope using the formula:
    /// Slope = (Y2 - Y1) / (X2 - X1) where X represents time and Y represents value.
    calculatedSlope = (values.back().first - oldest->first) / static_cast<double>(endTime - oldest->second);

    return true;
}



template<typename T>
bool VariableAnomalyMonitor<T>::GetAdvancedSlopeOverDeltaTime(uint32_t deltaTimeMsec, double &resultSlope, SlopeCalculationType calcType, uint32_t windowSize) noexcept {
    /// Check if there's any data to compute from or if the provided window size is invalid.
    if (values.empty() || windowSize < 2) {
        return false;
    }

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
    std::deque<std::pair<T, uint32_t>> window(it, it + windowSize);

    /// Calculate the average of the data values within the window.
    double previousAverage = std::accumulate(window.begin(), window.end(), 0.0,
        [](double acc, const std::pair<T, uint32_t>& p) { return acc + p.first; }) / windowSize;

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
}

//==============================================================//
/// HELPERS
//==============================================================//
template<typename T>
bool VariableAnomalyMonitor<T>::CheckAnomalyDuration(const std::deque<uint32_t>& anomaliesDeque, uint32_t anomalyDuration) {
    if (!anomaliesDeque.empty() && ((GetElapsedTimeMsec() - anomaliesDeque.front()) >= anomalyDuration)) {
        return true;
    }
    return false;
}

/// Helper function for cleaning deques of type std::deque<std::pair<T, uint32_t>>
template<typename T>
void VariableAnomalyMonitor<T>::CleanDeque(std::deque<std::pair<T, uint32_t>>& deque, uint32_t currentTime, uint32_t timeLimit) {
    while (!deque.empty() && ((currentTime - deque.front().second) > timeLimit)) {
        deque.pop_front();
    }
}

/// Helper function for cleaning deques of type std::deque<uint32_t>
template<typename T>
void VariableAnomalyMonitor<T>::CleanDeque(std::deque<uint32_t>& deque, uint32_t currentTime, uint32_t timeLimit) {
    while (!deque.empty() && ((currentTime - deque.front()) > timeLimit)) {
        deque.pop_front();
    }
}

template<typename T>
bool VariableAnomalyMonitor<T>::CalculateSlopeFromType(const std::vector<double> &slopes, SlopeCalculationType calcType, double &resultSlope) {
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


