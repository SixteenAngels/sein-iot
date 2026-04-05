/**
 * @file adc_hal.h
 * @brief ADC Hardware Abstraction Layer
 */

#ifndef ADC_HAL_H
#define ADC_HAL_H

#include <stdint.h>
#include <stdbool.h>
#include "esp_adc/adc_oneshot.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize ADC
 */
bool adc_hal_init(void);

/**
 * Read ADC channel
 */
uint16_t adc_hal_read(uint8_t channel);

/**
 * Read multiple ADC samples
 */
bool adc_hal_read_multiple(uint8_t channel, uint16_t *samples, uint16_t count);

/**
 * Deinitialize ADC
 */
bool adc_hal_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* ADC_HAL_H */
