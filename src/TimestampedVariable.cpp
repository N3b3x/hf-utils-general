/**
 *
 * @todo Add @copyright line once project copyright wording is finalised.
 */

#include <cfloat>
#include <cmath>
#include "TimestampedVariable.h"

// Template specialization for for comparing timestamped values.

template<>	bool TimestampedVariable<float>::operator==(const float& rhs) const noexcept
{
	return fabs(value- rhs) < FLT_EPSILON;
}
