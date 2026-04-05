/**
 * @file gpio_hal.c
 * @brief GPIO Hardware Abstraction Layer Implementation
 */

#include "gpio_hal.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "GPIO_HAL";

bool gpio_hal_init(uint8_t pin, gpio_hal_mode_t mode, gpio_hal_pull_t pull)
{
    gpio_mode_t esp_mode;
    switch (mode) {
    case GPIO_HAL_MODE_INPUT:
        esp_mode = GPIO_MODE_INPUT;
        break;
    case GPIO_HAL_MODE_OUTPUT:
        esp_mode = GPIO_MODE_OUTPUT;
        break;
    case GPIO_HAL_MODE_INPUT_OUTPUT:
        esp_mode = GPIO_MODE_INPUT_OUTPUT;
        break;
    case GPIO_HAL_MODE_ANALOG:
        esp_mode = GPIO_MODE_DISABLE;
        break;
    default:
        return false;
    }

    gpio_config_t cfg = {
        .pin_bit_mask = (1ULL << pin),
        .mode = esp_mode,
        .pull_up_en = (pull == GPIO_HAL_PULL_UP || pull == GPIO_HAL_PULL_UP_DOWN) ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE,
        .pull_down_en = (pull == GPIO_HAL_PULL_DOWN || pull == GPIO_HAL_PULL_UP_DOWN) ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    esp_err_t ret = gpio_config(&cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init GPIO %d: %d", pin, ret);
        return false;
    }

    return true;
}

bool gpio_hal_set_level(uint8_t pin, gpio_hal_level_t level)
{
    esp_err_t ret = gpio_set_level(pin, (uint32_t)level);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set level on GPIO %d: %d", pin, ret);
        return false;
    }
    return true;
}

gpio_hal_level_t gpio_hal_get_level(uint8_t pin)
{
    return (gpio_hal_level_t)gpio_get_level(pin);
}

bool gpio_hal_set_interrupt(uint8_t pin, bool enable)
{
    gpio_intr_type_t intr_type = enable ? GPIO_INTR_ANYEDGE : GPIO_INTR_DISABLE;
    esp_err_t ret = gpio_set_intr_type(pin, intr_type);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set interrupt on GPIO %d: %d", pin, ret);
        return false;
    }
    return true;
}

bool gpio_hal_deinit(uint8_t pin)
{
    esp_err_t ret = gpio_reset_pin(pin);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to deinit GPIO %d: %d", pin, ret);
        return false;
    }
    return true;
}
