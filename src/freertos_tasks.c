/**
 * @file freertos_tasks.c
 * @brief FreeRTOS Task Implementations
 */

#include "freertos_tasks.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"

static const char *TAG = "TASKS";

/* Task handles */
static TaskHandle_t sensor_task_handle = NULL;
static TaskHandle_t fpga_accel_task_handle = NULL;
static TaskHandle_t inference_task_handle = NULL;
static TaskHandle_t wifi_task_handle = NULL;
static TaskHandle_t mqtt_task_handle = NULL;
static TaskHandle_t security_task_handle = NULL;
static TaskHandle_t display_task_handle = NULL;
static TaskHandle_t power_mon_task_handle = NULL;

void sensor_task(void *pvParameters)
{
    (void)pvParameters;
    ESP_LOGI(TAG, "Sensor task started");

    while (1) {
        /* Acquire sensor data */
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void fpga_accel_task(void *pvParameters)
{
    (void)pvParameters;
    ESP_LOGI(TAG, "FPGA acceleration task started");

    while (1) {
        /* Process FPGA acceleration requests */
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void inference_task(void *pvParameters)
{
    (void)pvParameters;
    ESP_LOGI(TAG, "Inference task started");

    while (1) {
        /* Run TinyML inference */
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void wifi_task(void *pvParameters)
{
    (void)pvParameters;
    ESP_LOGI(TAG, "WiFi task started");

    while (1) {
        /* WiFi connection management */
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void mqtt_task(void *pvParameters)
{
    (void)pvParameters;
    ESP_LOGI(TAG, "MQTT task started");

    while (1) {
        /* MQTT publish/subscribe */
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void security_task(void *pvParameters)
{
    (void)pvParameters;
    ESP_LOGI(TAG, "Security task started");

    while (1) {
        /* Cryptographic operations */
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void display_task(void *pvParameters)
{
    (void)pvParameters;
    ESP_LOGI(TAG, "Display task started");

    while (1) {
        /* Update display */
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void power_mon_task(void *pvParameters)
{
    (void)pvParameters;
    ESP_LOGI(TAG, "Power monitoring task started");

    while (1) {
        /* Monitor battery and power */
        vTaskDelay(pdMS_TO_TICKS(60000));  /* 60 seconds */
    }
}

void freertos_tasks_init(void)
{
    ESP_LOGI(TAG, "Creating FreeRTOS tasks");

    /* Create tasks on Core 1 (PRO core) */
    xTaskCreatePinnedToCore(
        sensor_task,
        "sensor_task",
        3072,
        NULL,
        5,
        &sensor_task_handle,
        1
    );

    xTaskCreatePinnedToCore(
        fpga_accel_task,
        "fpga_accel_task",
        4096,
        NULL,
        6,
        &fpga_accel_task_handle,
        1
    );

    xTaskCreatePinnedToCore(
        inference_task,
        "inference_task",
        4096,
        NULL,
        4,
        &inference_task_handle,
        1
    );

    xTaskCreatePinnedToCore(
        display_task,
        "display_task",
        2048,
        NULL,
        2,
        &display_task_handle,
        1
    );

    /* Create tasks on Core 0 (APP core) */
    xTaskCreatePinnedToCore(
        wifi_task,
        "wifi_task",
        8192,
        NULL,
        3,
        &wifi_task_handle,
        0
    );

    xTaskCreatePinnedToCore(
        mqtt_task,
        "mqtt_task",
        4096,
        NULL,
        3,
        &mqtt_task_handle,
        0
    );

    xTaskCreatePinnedToCore(
        security_task,
        "security_task",
        3072,
        NULL,
        7,
        &security_task_handle,
        0
    );

    xTaskCreatePinnedToCore(
        power_mon_task,
        "power_mon_task",
        2048,
        NULL,
        1,
        &power_mon_task_handle,
        0
    );

    ESP_LOGI(TAG, "All tasks created successfully");
}
