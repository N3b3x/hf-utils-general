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
 * @brief Contains the LineEstimator class for estimating the slope of a line from data points
 */

#include <vector>
#include <utility> // for std::pair
#include <numeric> // for std::accumulate

/**
 * @class LineEstimator
 * @brief A class for estimating the slope of a line from data points
 *
 * This class allows you to add and clear data points, and estimate the average slope of the line formed by these points.
 */
class LineEstimator {
public:
    /**
     * @brief Adds a new data point to the data vector
     * @param x The x-coordinate of the data point
     * @param y The y-coordinate of the data point
     */
    void AddPoint(float x, float y) {
        data.push_back(std::make_pair(x, y));
    }

    /**
     * @brief Clears all data points from the data vector
     */
    void ClearPoints() {
        data.clear();
    }

    /**
     * @brief Estimates the average slope of the line formed by the data points
     * @return The estimated slope of the line. Returns 0 if there are not enough data points to estimate the slope.
     *
     * This method implements the formula for the slope of a line in a least squares sense.
     */
    float EstimateSlope() {
        if (data.size() < 2) {
            return 0;
        }

        float x_sum = 0.0, y_sum = 0.0, xy_sum = 0.0, xx_sum = 0.0;
        for (const auto& point : data) {
            x_sum += point.first;
            y_sum += point.second;
            xy_sum += point.first * point.second;
            xx_sum += point.first * point.first;
        }

        float n = data.size();
        float slope = (n * xy_sum - x_sum * y_sum) / (n * xx_sum - x_sum * x_sum);
        return slope;
    }

private:
    /**
     * @brief A vector of pairs to store the data points
     */
    std::vector<std::pair<float, float>> data;

};

#endif /* UTILITIES_COMMON_LINEESTIMATOR_H_ */
