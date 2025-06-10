/**
 * @file ValueMonitor.cpp
 * @brief Helper functions to simplify the template code.
 */


#include "VariableMonitor.h"

bool IsSlopeAnomaly( float deltaValue, float deltaTimeMsec, float slopeLimit, SlopeType slopeType, AnomalyType slopeAnomalyType ) noexcept
{
	if( deltaTimeMsec >= 0.0F )
	{
		if( slopeType == SlopeType::Absolute ) /// Use absolute values for comparison
		{
			const float calculatedSlope = std::fabs( deltaValue / deltaTimeMsec );
			if( slopeAnomalyType == AnomalyType::AboveLimit )
			{
				return calculatedSlope > std::fabs(slopeLimit);
			}
			else // slopeAnomalyType == AnomalyType::BelowLimit )
			{
				return calculatedSlope < std::fabs(slopeLimit);
			}
		}
		else /// Use actual values for comparison
		{
			const float calculatedSlope = deltaValue / deltaTimeMsec;

			if( slopeAnomalyType == AnomalyType::AboveLimit )
			{
				return calculatedSlope > slopeLimit;
			}
			else // slopeAnomalyType == AnomalyType::BelowLimit )
			{
				return calculatedSlope < slopeLimit;
			}
		}
	}
	return false;
}

