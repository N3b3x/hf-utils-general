/**
 *
 * Nebula Tech Corporation
 *
 * Copyright © 2023 Nebula Tech Corporation. All Rights Reserved.
 * This file is part of HardFOC and is licensed under the GNU General Public License v3.0 or later.
 *
 */

#ifndef UTILITIES_COMMON_BOUNDEDLINEARCURVE_H_
#define UTILITIES_COMMON_BOUNDEDLINEARCURVE_H_

#include <algorithm> /// For std::swap

/**
 * @brief Class to represent a linear equation with a specific x-axis range.
 */
class BoundedLinearCurve {
public:
    /**
     * @brief Default constructor to initialize the linear bound with default values.
     */
    BoundedLinearCurve() noexcept
        : slope(0.0f), intercept(0.0f), xMin(0.0f), xMax(1.0f), epsilon(1e-9f)
    { }

    /**
     * @brief Constructor with parameters to initialize the linear bound.
     * @param m The slope of the linear equation.
     * @param b The y-intercept of the linear equation.
     * @param xMin The minimum x value for this bound.
     * @param xMax The maximum x value for this bound.
     * @param eps The tolerance for floating-point comparison.
     */
    BoundedLinearCurve(float mArg, float bArg, float xMinArg, float xMaxArg, float epsArg = 1e-9f) noexcept
        : slope(mArg), intercept(bArg), xMin(xMinArg), xMax(xMaxArg), epsilon(epsArg)
    {
        /// Ensure xMin is less than xMax, swap if necessary
        if (xMin > xMax) {
            std::swap(this->xMin, this->xMax);
        }
    }

    /**
     * @brief Constructor to initialize the linear bound using two points and x-axis range.
     * @param x1 The x value of the first point.
     * @param y1 The y value of the first point.
     * @param x2 The x value of the second point.
     * @param y2 The y value of the second point.
     * @param xMin The minimum x value for this bound.
     * @param xMax The maximum x value for this bound.
     * @param eps The tolerance for floating-point comparison.
     */
    BoundedLinearCurve(float x1, float y1, float x2, float y2, float xMinArg, float xMaxArg, float epsArg = 1e-9f) noexcept
        : xMin(xMinArg), xMax(xMaxArg), epsilon(epsArg)
    {
        /// Calculate slope (m) and intercept (b)
        slope = (y2 - y1) / (x2 - x1);
        intercept = y1 - slope * x1;

        /// Ensure xMin is less than xMax, swap if necessary
        if (xMin > xMax) {
            std::swap(this->xMin, this->xMax);
        }
    }

    /**
     * @brief Calculates the y value for a given x using this linear equation.
     * @param x The x value.
     * @return The calculated y value.
     */
    float CalculateY(float x) const noexcept {
        return slope * (x) + intercept;
    }

    /**
     * @brief Checks if a given x value is within the range of this bound.
     * @param x The x value to check.
     * @return True if x is within [xMin, xMax] or close within epsilon, false otherwise.
     */
    bool InRange(float x) const noexcept {
        return (x >= xMin - epsilon) && (x <= xMax + epsilon);
    }

	//=========================================//
    /// Getter methods for member variables
	//=========================================//

    /**
     * @brief Gets the slope of the linear equation.
     * @return The slope (m) of the linear equation.
     */
    float GetSlope() const noexcept { return slope; }

    /**
     * @brief Gets the y-intercept of the linear equation.
     * @return The y-intercept (b) of the linear equation.
     */
    float GetIntercept() const noexcept { return intercept; }

    /**
     * @brief Gets the minimum x value for which this bound is valid.
     * @return The minimum x value.
     */
    float GetXMin() const noexcept { return xMin; }

    /**
     * @brief Gets the maximum x value for which this bound is valid.
     * @return The maximum x value.
     */
    float GetXMax() const noexcept { return xMax; }

    /**
     * @brief Gets the tolerance for floating-point comparison.
     * @return The tolerance value.
     */
    float GetEpsilon() const noexcept { return epsilon; }

	//=========================================//
	/// Setter methods for member variables
	//=========================================//

    /**
     * @brief Sets the slope of the linear equation.
     * @param m The new slope value.
     */
    void SetSlope(float m) noexcept { slope = m; }

    /**
     * @brief Sets the y-intercept of the linear equation.
     * @param b The new y-intercept value.
     */
    void SetIntercept(float b) noexcept { intercept = b; }

    /**
     * @brief Sets the minimum x value for which this bound is valid.
     * @param xMinArg The new minimum x value.
     */
    void SetXMin(float xMinArg) noexcept {
        xMin = xMinArg;
        if (xMin > xMax) {
            std::swap(this->xMin, this->xMax);
        }
    }

    /**
     * @brief Sets the maximum x value for which this bound is valid.
     * @param xMaxArg The new maximum x value.
     */
    void SetXMax(float xMaxArg) noexcept {
        xMax = xMaxArg;
        if (xMin > xMax) {
            std::swap(this->xMin, this->xMax);
        }
    }

    /**
     * @brief Sets the tolerance for floating-point comparison.
     * @param eps The new tolerance value.
     */
    void SetEpsilon(float eps) noexcept { epsilon = eps; }

private:
    float slope;      ///< The slope (m) of the linear equation y = mx + b.
    float intercept;  ///< The y-intercept (b) of the linear equation y = mx + b.
    float xMin;       ///< The minimum x value for which this bound is valid.
    float xMax;       ///< The maximum x value for which this bound is valid.
    float epsilon;    ///< Tolerance to handle precision issues in floating-point comparison.
};


#endif /* UTILITIES_COMMON_BOUNDEDLINEARCURVE_H_ */
