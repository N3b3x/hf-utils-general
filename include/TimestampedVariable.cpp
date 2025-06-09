/**
 *
 * Nebula Tech Corporation
 *
 * Copyright © 2025 Nebula Tech Corporation. All Rights Reserved.
 * This file is part of HardFOC and is licensed under the GNU General Public License v3.0 or later.
 *
 */

#include"UTILITIES/common/TimeStampedVariable.h"

#include <cfloat>

// Template specialization for for comparing timestamped values.

template<>	bool TimestampedVariable<float>::operator==(const float& rhs) const noexcept
{
	return fabs(value- rhs) < FLT_EPSILON;
}
