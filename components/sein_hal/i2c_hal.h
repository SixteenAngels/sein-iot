/**
 * @file i2c_hal.h
 * @brief I2C Hardware Abstraction Layer
 */

#ifndef I2C_HAL_H
#define I2C_HAL_H

#include <stdint.h>
#include <stdbool.h>
#include "driver/i2c_master.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize I2C bus
 */
bool i2c_hal_init(uint32_t clock_speed_hz);

/**
 * Write to I2C device
 */
bool i2c_hal_write(uint8_t addr, const uint8_t *data, uint16_t len);

/**
 * Read from I2C device
 */
bool i2c_hal_read(uint8_t addr, uint8_t *data, uint16_t len);

/**
 * Write then read (read with restart)
 */
bool i2c_hal_write_read(uint8_t addr,
                       const uint8_t *tx_data, uint16_t tx_len,
                       uint8_t *rx_data, uint16_t rx_len);

/**
 * Deinitialize I2C bus
 */
bool i2c_hal_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* I2C_HAL_H */
