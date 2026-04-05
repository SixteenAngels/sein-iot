/**
 * @file adc_hal.c
 * @brief ADC Hardware Abstraction Layer Implementation
 */

#include "adc_hal.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"

static const char *TAG = "ADC_HAL";

static adc_oneshot_unit_handle_t adc_handle = NULL;

bool adc_hal_init(void)
{
    if (adc_handle != NULL) {
        return true;  /* Already initialized */
    }

    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };

    esp_err_t ret = adc_oneshot_new_unit(&init_config, &adc_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create ADC unit: %d", ret);
        return false;
    }

    /* Configure ADC channel */
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_11,
    };

    /* Configure channels 0-6 */
    for (int i = 0; i < 7; i++) {
        ret = adc_oneshot_config_channel(adc_handle, i, &config);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to config ADC channel %d: %d", i, ret);
            return false;
        }
    }

    ESP_LOGI(TAG, "ADC initialized");
    return true;
}

uint16_t adc_hal_read(uint8_t channel)
{
    if (adc_handle == NULL) {
        ESP_LOGE(TAG, "ADC not initialized");
        return 0;
    }

    int adc_raw;
    esp_err_t ret = adc_oneshot_read(adc_handle, channel, &adc_raw);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "ADC read failed: %d", ret);
        return 0;
    }

    return (uint16_t)adc_raw;
}

bool adc_hal_read_multiple(uint8_t channel, uint16_t *samples, uint16_t count)
{
    if (adc_handle == NULL || samples == NULL || count == 0) {
        return false;
    }

    for (uint16_t i = 0; i < count; i++) {
        int adc_raw;
        esp_err_t ret = adc_oneshot_read(adc_handle, channel, &adc_raw);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "ADC read failed: %d", ret);
            return false;
        }
        samples[i] = (uint16_t)adc_raw;
    }

    return true;
}

bool adc_hal_deinit(void)
{
    if (adc_handle != NULL) {
        esp_err_t ret = adc_oneshot_del_unit(adc_handle);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to delete ADC unit: %d", ret);
            return false;
        }
        adc_handle = NULL;
    }
    return true;
}
