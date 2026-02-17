/**
 *
 * Nebula Tech Corporation
 *
 * Copyright © 2023 Nebula Tech Corporation. All Rights Reserved.
 * This file is part of HardFOC and is licensed under the GNU General Public License v3.0 or later.
 *
 */

#ifndef UTILITIES_COMMON_LINEESTIMATOR_H_
#define UTILITIES_COMMON_LINEESTIMATOR_H_

/**
 * @file LineEstimator.h
 * @brief Contains the LineEstimator class for estimating the slope of a line from data points.
 */

#include <array>
#include <utility> // for std::pair
#include <cstddef>

/**
 * @class LineEstimator
 * @brief A class for estimating the slope of a line from data points.
 *
 * Uses a fixed-capacity std::array so no heap allocation is required.
 *
 * @tparam MaxPoints  Maximum number of data points the estimator can hold.
 */
template <size_t MaxPoints = 64>
class LineEstimator {
public:
    /**
     * @brief Adds a new data point.
     * @param x The x-coordinate of the data point.
     * @param y The y-coordinate of the data point.
     * @return true if the point was added, false if at capacity.
     */
    bool AddPoint(float x, float y) {
        if (count_ >= MaxPoints) {
            return false;
        }
        data_[count_++] = {x, y};
        return true;
    }

    /**
     * @brief Clears all data points.
     */
    void ClearPoints() {
        count_ = 0;
    }

    /**
     * @brief Returns the current number of stored data points.
     */
    [[nodiscard]] size_t Size() const { return count_; }

    /**
     * @brief Returns the maximum capacity.
     */
    [[nodiscard]] static constexpr size_t Capacity() { return MaxPoints; }

    /**
     * @brief Estimates the average slope of the line formed by the data points.
     * @return The estimated slope of the line. Returns 0 if there are fewer than 2 data points.
     *
     * Implements the formula for the slope of a line in a least squares sense.
     */
    float EstimateSlope() const {
        if (count_ < 2) {
            return 0.0f;
        }

        float x_sum = 0.0f, y_sum = 0.0f, xy_sum = 0.0f, xx_sum = 0.0f;
        for (size_t i = 0; i < count_; ++i) {
            x_sum += data_[i].first;
            y_sum += data_[i].second;
            xy_sum += data_[i].first * data_[i].second;
            xx_sum += data_[i].first * data_[i].first;
        }

        float n = static_cast<float>(count_);
        float denom = n * xx_sum - x_sum * x_sum;
        if (denom == 0.0f) {
            return 0.0f;
        }
        return (n * xy_sum - x_sum * y_sum) / denom;
    }

private:
    std::array<std::pair<float, float>, MaxPoints> data_{};
    size_t count_ = 0;
};

#endif /* UTILITIES_COMMON_LINEESTIMATOR_H_ */
