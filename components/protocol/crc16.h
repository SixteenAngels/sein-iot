/**
 * @file crc16.h
 * @brief CRC-16-CCITT Implementation for SPI Protocol
 * 
 * Polynomial: 0x1021 (x^16 + x^12 + x^5 + 1)
 * Initial value: 0xFFFF
 * Final XOR: None
 * Bit order: MSB first
 */

#ifndef CRC16_H
#define CRC16_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* CRC-16-CCITT Parameters */
#define CRC16_POLYNOMIAL    0x1021
#define CRC16_INIT_VALUE    0xFFFF

/**
 * Compute CRC-16-CCITT for a data buffer
 * 
 * @param data      Pointer to data buffer
 * @param length    Length of data buffer in bytes
 * @return          Computed CRC-16 value
 */
uint16_t crc16_compute(const uint8_t *data, size_t length);

/**
 * Compute CRC-16-CCITT with initial value
 * 
 * @param data      Pointer to data buffer
 * @param length    Length of data buffer in bytes
 * @param crc_init  Initial CRC value
 * @return          Computed CRC-16 value
 */
uint16_t crc16_compute_init(const uint8_t *data, size_t length, uint16_t crc_init);

/**
 * Verify CRC-16-CCITT against provided value
 * 
 * @param data      Pointer to data buffer
 * @param length    Length of data buffer in bytes
 * @param expected_crc  Expected CRC-16 value
 * @return          true if CRC matches, false otherwise
 */
bool crc16_verify(const uint8_t *data, size_t length, uint16_t expected_crc);

#ifdef __cplusplus
}
#endif

#endif /* CRC16_H */
