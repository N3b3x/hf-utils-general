/**
 *
 * Nebula Tech Corporation
 *
 * Copyright © 2023 Nebula Tech Corporation. All Rights Reserved.
 * This file is part of HardFOC and is licensed under the GNU General Public License v3.0 or later.
 *
 */

#include <stdint.h>
#include <stddef.h>

#include "CrcCalculator.h"

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

uint16_t crc16(const void* data, uint32_t count)
{
    const uint8_t* pData = (const uint8_t*)data;
    uint32_t crc = 0x0000FFFF;
    
    if( data != NULL )
    {
        for(int j = 0; j< (int)count; j++)
        {
            crc = ((crc >> 8) | (crc << 8)) & 0xffff;
            crc ^= (pData[j] & 0xff);
            crc ^= ((crc & 0xff) >> 4);
            crc ^= (crc << 12) & 0xffff;
            crc ^= ((crc & 0xff) << 5) & 0xffff;
        }
    }
    crc &= 0xffff;

    return (uint16_t)(crc & 0x0000FFFF);
}

