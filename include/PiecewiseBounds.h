/**
 *
 * Nebula Tech Corporation
 *
 * Copyright © 2023 Nebula Tech Corporation. All Rights Reserved.
 * This file is part of HardFOC and is licensed under the GNU General Public License v3.0 or later.
 *
 */

#ifndef UTILITIES_COMMON_PIECEWISEBOUNDS_H_
#define UTILITIES_COMMON_PIECEWISEBOUNDS_H_

#include "UTILITIES/common/BoundedLinearCurve.h"
#include "UTILITIES/common/DynamicArray.h"
#include "UTILITIES/common/PiecewiseLinearCurve.h"

/**
 * @brief Class to represent piecewise maximum and minimum bounds using multiple BoundedLinearCurve segments.
 * @tparam MaxMaxSegments The maximum number of segments allowed in the piecewise maximum bounds.
 * @tparam MaxMinSegments The maximum number of segments allowed in the piecewise minimum bounds.
 */
template <uint8_t MaxMaxSegments, uint8_t MaxMinSegments>
class PiecewiseBounds {
public:
    /**
     * @brief Constructor to initialize the piecewise bounds with global bounds.
     * @param globalYMin The global minimum y value for the piecewise bounds.
     * @param globalYMax The global maximum y value for the piecewise bounds.
     */
    PiecewiseBounds(float globalYMinArg, float globalYMaxArg) noexcept
        : globalYMin(globalYMinArg), globalYMax(globalYMaxArg) {}

    /**
     * @brief Adds a new BoundedLinearCurve segment to the maximum bounds.
     * @param segment The BoundedLinearCurve segment to add.
     * @return True if the segment was added successfully, false otherwise.
     */
    bool AddMaxSegment(const BoundedLinearCurve& segment) {
        return maxSegments.AddSegment(segment);
    }

    /**
     * @brief Adds a new BoundedLinearCurve segment to the minimum bounds.
     * @param segment The BoundedLinearCurve segment to add.
     * @return True if the segment was added successfully, false otherwise.
     */
    bool AddMinSegment(const BoundedLinearCurve& segment) {
        return minSegments.AddSegment(segment);
    }

    /**
     * @brief Calculates the maximum y value for a given x using the piecewise maximum bounds.
     * @param x The x value.
     * @param y The reference to store the calculated y value.
     * @return True if the calculation was successful, false otherwise.
     */
    bool CalculateMaxY(float x, float& y) const {
        if (maxSegments.CalculateY(x, y)) {
            return true;
        }
        /// Handle the case where x is out of range of all segments
        y = globalYMax;
        return false;
    }

    /**
     * @brief Calculates the minimum y value for a given x using the piecewise minimum bounds.
     * @param x The x value.
     * @param y The reference to store the calculated y value.
     * @return True if the calculation was successful, false otherwise.
     */
    bool CalculateMinY(float x, float& y) const {
        if (minSegments.CalculateY(x, y)) {
            return true;
        }
        /// Handle the case where x is out of range of all segments
        y = globalYMin;
        return false;
    }

    /**
     * @brief Clears all segments from the piecewise maximum and minimum bounds.
     */
    void ClearAllSegments() {
        maxSegments.ClearAllSegments();
        minSegments.ClearAllSegments();
    }

private:
    float globalYMin; ///< The global minimum y value for the piecewise bounds.
    float globalYMax; ///< The global maximum y value for the piecewise bounds.
    PiecewiseBoundedLinearCurve<MaxMaxSegments> maxSegments; ///< Piecewise curve for maximum bounds.
    PiecewiseBoundedLinearCurve<MaxMinSegments> minSegments; ///< Piecewise curve for minimum bounds.
};

#endif // UTILITIES_COMMON_PIECEWISEBOUNDS_H_
