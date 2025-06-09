/**
 *
 * Nebula Tech Corporation
 *
 * Copyright © 2023 Nebula Tech Corporation. All Rights Reserved.
 * This file is part of HardFOC and is licensed under the GNU General Public License v3.0 or later.
 *
 */

#ifndef UTILITIES_COMMON_PIECEWISEBOUNDEDLINEARCURVE_H_
#define UTILITIES_COMMON_PIECEWISEBOUNDEDLINEARCURVE_H_

#include "UTILITIES/common/BoundedLinearCurve.h"
#include "UTILITIES/common/DynamicArray.h"

/**
 * @brief Class to represent a piecewise linear curve using multiple BoundedLinearCurve segments.
 * @tparam MaxSegments The maximum number of segments allowed in the piecewise curve.
 */
template <uint8_t MaxSegments>
class PiecewiseBoundedLinearCurve {
public:
    /**
     * @brief Constructor to initialize the piecewise linear curve.
     */
    PiecewiseBoundedLinearCurve() = default;

    /**
     * @brief Adds a new BoundedLinearCurve segment to the piecewise curve.
     * @param segment The BoundedLinearCurve segment to add.
     * @return True if the segment was added successfully, false otherwise.
     */
    bool AddSegment(const BoundedLinearCurve& segment) {
        return segments.Append(segment);
    }

    /**
     * @brief Calculates the y value for a given x using the piecewise linear curve.
     * @param x The x value.
     * @param y The reference to store the calculated y value.
     * @return True if the calculation was successful, false otherwise.
     */
    bool CalculateY(float x, float& y) const {
        for (const auto& segment : segments) {
            if (segment.InRange(x)) {
                y = segment.CalculateY(x);
                return true;
            }
        }
        // Handle the case where x is out of range of all segments
        return false;
    }

    /**
     * @brief Clears all segments from the piecewise linear curve.
     */
    void ClearAllSegments() {
        segments.ClearAll();
    }

private:
    DynamicArray<BoundedLinearCurve, MaxSegments> segments; ///< Array to store the segments.
};

#endif // UTILITIES_COMMON_PIECEWISEBOUNDEDLINEARCURVE_H_
