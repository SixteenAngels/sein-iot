/**
 * @file spi_hal.h
 * @brief SPI Hardware Abstraction Layer
 */

#ifndef SPI_HAL_H
#define SPI_HAL_H

#include <stdint.h>
#include <stdbool.h>
#include "driver/spi_master.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize SPI master
 */
bool spi_hal_init(uint32_t clock_speed_hz);

/**
 * SPI write operation
 */
bool spi_hal_write(const uint8_t *data, uint16_t len);

/**
 * SPI read operation
 */
bool spi_hal_read(uint8_t *data, uint16_t len);

/**
 * SPI write/read operation (full duplex)
 */
bool spi_hal_xfer(const uint8_t *tx_data, uint8_t *rx_data, uint16_t len);

/**
 * Deinitialize SPI
 */
bool spi_hal_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* SPI_HAL_H */
