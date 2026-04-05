/**
 * @file i2c_hal.c
 * @brief I2C Hardware Abstraction Layer Implementation
 */

#include "i2c_hal.h"
#include "driver/i2c_master.h"
#include "esp_log.h"

static const char *TAG = "I2C_HAL";

static i2c_master_bus_handle_t i2c_bus_handle = NULL;

bool i2c_hal_init(uint32_t clock_speed_hz)
{
    if (i2c_bus_handle != NULL) {
        return true;  /* Already initialized */
    }

    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_NUM_0,
        .scl_io_num = 9,       /* GPIO9 */
        .sda_io_num = 8,       /* GPIO8 */
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    esp_err_t ret = i2c_new_master_bus(&i2c_mst_config, &i2c_bus_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create I2C bus: %d", ret);
        return false;
    }

    ESP_LOGI(TAG, "I2C bus initialized at %lu Hz", clock_speed_hz);
    return true;
}

bool i2c_hal_write(uint8_t addr, const uint8_t *data, uint16_t len)
{
    if (i2c_bus_handle == NULL) {
        ESP_LOGE(TAG, "I2C bus not initialized");
        return false;
    }

    esp_err_t ret = i2c_master_transmit(i2c_bus_handle, addr, data, len, -1);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C write failed: %d", ret);
        return false;
    }

    return true;
}

bool i2c_hal_read(uint8_t addr, uint8_t *data, uint16_t len)
{
    if (i2c_bus_handle == NULL) {
        ESP_LOGE(TAG, "I2C bus not initialized");
        return false;
    }

    esp_err_t ret = i2c_master_receive(i2c_bus_handle, addr, data, len, -1);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C read failed: %d", ret);
        return false;
    }

    return true;
}

bool i2c_hal_write_read(uint8_t addr,
                       const uint8_t *tx_data, uint16_t tx_len,
                       uint8_t *rx_data, uint16_t rx_len)
{
    if (i2c_bus_handle == NULL) {
        ESP_LOGE(TAG, "I2C bus not initialized");
        return false;
    }

    esp_err_t ret = i2c_master_transmit_receive(i2c_bus_handle, addr,
                                                 tx_data, tx_len,
                                                 rx_data, rx_len, -1);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C write/read failed: %d", ret);
        return false;
    }

    return true;
}

bool i2c_hal_deinit(void)
{
    if (i2c_bus_handle != NULL) {
        esp_err_t ret = i2c_del_master_bus(i2c_bus_handle);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to delete I2C bus: %d", ret);
            return false;
        }
        i2c_bus_handle = NULL;
    }
    return true;
}
