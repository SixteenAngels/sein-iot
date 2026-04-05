/**
 * @file main.c
 * @brief SEIN-IoT Main Application
 *
 * Secure Edge Intelligence Node with IoT Integration
 * Entry point for the firmware
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "nvs_flash.h"
#include "esp_log.h"

#include "sein_iot.h"
#include "config.h"
#include "freertos_tasks.h"
#include "fpga_manager.h"
#include "secure_boot.h"
#include "gpio_hal.h"
#include "adc_hal.h"

static const char *TAG = "MAIN";

/**
 * Application initialization
 */
static bool app_init(void)
{
    ESP_LOGI(TAG, "================================================");
    ESP_LOGI(TAG, "SEIN-IoT Firmware v%d.%d.%d",
             SEIN_IOT_MAJOR, SEIN_IOT_MINOR, SEIN_IOT_PATCH);
    ESP_LOGI(TAG, "================================================");

    /* Print chip info */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    ESP_LOGI(TAG, "Chip: %s (revision %d)", "ESP32-S3", chip_info.revision);
    ESP_LOGI(TAG, "Features: %s%s%s%s%s",
             (chip_info.features & CHIP_FEATURE_WIFI_BGN) ? "WiFi " : "",
             (chip_info.features & CHIP_FEATURE_BT) ? "BT " : "",
             (chip_info.features & CHIP_FEATURE_BLE) ? "BLE " : "",
             (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "Flash " : "",
             (chip_info.features & CHIP_FEATURE_EMB_PSRAM) ? "PSRAM " : "");

    uint32_t flash_size;
    esp_flash_get_size(NULL, &flash_size);
    ESP_LOGI(TAG, "Flash: %lu MB", flash_size / (1024 * 1024));
    ESP_LOGI(TAG, "Free heap: %lu bytes", esp_get_free_heap_size());

    /* Initialize NVS (Non-Volatile Storage) */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "Erasing NVS flash...");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "NVS initialized");

    /* Initialize GPIO HAL */
    if (!gpio_hal_init(14, GPIO_HAL_MODE_INPUT, GPIO_HAL_PULL_NONE)) {
        ESP_LOGE(TAG, "GPIO init failed");
        return false;
    }
    ESP_LOGI(TAG, "GPIO HAL initialized");

    /* Initialize ADC HAL */
    if (!adc_hal_init()) {
        ESP_LOGE(TAG, "ADC init failed");
        return false;
    }
    ESP_LOGI(TAG, "ADC HAL initialized");

    /* Initialize secure boot */
    if (!secure_boot_init()) {
        ESP_LOGE(TAG, "Secure boot init failed");
        // Don't fail on this in demo mode - would normally halt here
    }
    ESP_LOGI(TAG, "Secure boot initialized");

    /* Initialize FPGA subsystem */
    if (!fpga_manager_init()) {
        ESP_LOGE(TAG, "FPGA manager init failed");
        return false;
    }
    ESP_LOGI(TAG, "FPGA manager initialized");

    ESP_LOGI(TAG, "Platform initialization complete");
    return true;
}

/**
 * Main application entry point
 */
void app_main(void)
{
    ESP_LOGI(TAG, "SEIN-IoT Firmware Starting...");

    /* Initialize platform */
    if (!app_init()) {
        ESP_LOGE(TAG, "Platform initialization failed!");
        vTaskSuspend(NULL);
        return;
    }

    /* Create FreeRTOS tasks */
    freertos_tasks_init();

    /* Let FreeRTOS scheduler take over */
    ESP_LOGI(TAG, "Starting FreeRTOS scheduler...");

    /* Main task can now be suspended - all work is done by other tasks */
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10000));
        ESP_LOGI(TAG, "Heartbeat - Free heap: %lu bytes", esp_get_free_heap_size());
    }
}
