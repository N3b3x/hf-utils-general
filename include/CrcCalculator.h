/**
 *
 * Nebula Tech Corporation
 *
 * Copyright © 2023 Nebula Tech Corporation. All Rights Reserved.
 * This file is part of HardFOC and is licensed under the GNU General Public License v3.0 or later.
 *
 */

#ifndef UTILITY_FUNCTIONS_CRCCALCULATOR_H_
#define UTILITY_FUNCTIONS_CRCCALCULATOR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief Calculates the CRC for the input data buffer using the CRC-16/CCITT-False Algorithm.
 *
 * This function computes the Cyclic Redundancy Check (CRC) for a given data buffer using the CRC-16/CCITT-False Algorithm.
 * The calculated CRC can be used to detect accidental changes to raw data.
 *
 * @param pData Pointer to the input data buffer.
 * @param count Size of the input data buffer.
 * @return CRC value as a 16-bit unsigned integer.
 */
uint16_t crc16( const void* pData, uint32_t count);


#ifdef __cplusplus
}
#endif

#endif /* UTILITY_FUNCTIONS_CRCCALCULATOR_H_ */
