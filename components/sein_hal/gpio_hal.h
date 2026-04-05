/**
 * @file gpio_hal.h
 * @brief GPIO Hardware Abstraction Layer
 */

#ifndef GPIO_HAL_H
#define GPIO_HAL_H

#include <stdint.h>
#include <stdbool.h>
#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    GPIO_HAL_MODE_INPUT = 0,
    GPIO_HAL_MODE_OUTPUT = 1,
    GPIO_HAL_MODE_INPUT_OUTPUT = 2,
    GPIO_HAL_MODE_ANALOG = 3,
} gpio_hal_mode_t;

typedef enum {
    GPIO_HAL_LEVEL_LOW = 0,
    GPIO_HAL_LEVEL_HIGH = 1,
} gpio_hal_level_t;

typedef enum {
    GPIO_HAL_PULL_NONE = 0,
    GPIO_HAL_PULL_UP = 1,
    GPIO_HAL_PULL_DOWN = 2,
    GPIO_HAL_PULL_UP_DOWN = 3,
} gpio_hal_pull_t;

/**
 * Initialize GPIO pin
 */
bool gpio_hal_init(uint8_t pin, gpio_hal_mode_t mode, gpio_hal_pull_t pull);

/**
 * Set GPIO pin output level
 */
bool gpio_hal_set_level(uint8_t pin, gpio_hal_level_t level);

/**
 * Get GPIO pin input level
 */
gpio_hal_level_t gpio_hal_get_level(uint8_t pin);

/**
 * Configure GPIO interrupt
 */
bool gpio_hal_set_interrupt(uint8_t pin, bool enable);

/**
 * Deinitialize GPIO pin
 */
bool gpio_hal_deinit(uint8_t pin);

#ifdef __cplusplus
}
#endif

#endif /* GPIO_HAL_H */
