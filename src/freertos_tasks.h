/**
 * @file freertos_tasks.h
 * @brief FreeRTOS Task Definitions
 */

#ifndef FREERTOS_TASKS_H
#define FREERTOS_TASKS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize all FreeRTOS tasks
 */
void freertos_tasks_init(void);

/**
 * Sensor acquisition task
 */
void sensor_task(void *pvParameters);

/**
 * FPGA acceleration task
 */
void fpga_accel_task(void *pvParameters);

/**
 * TinyML inference task
 */
void inference_task(void *pvParameters);

/**
 * WiFi management task
 */
void wifi_task(void *pvParameters);

/**
 * MQTT communication task
 */
void mqtt_task(void *pvParameters);

/**
 * Security operations task
 */
void security_task(void *pvParameters);

/**
 * Display/UI task
 */
void display_task(void *pvParameters);

/**
 * Power monitoring task
 */
void power_mon_task(void *pvParameters);

#ifdef __cplusplus
}
#endif

#endif /* FREERTOS_TASKS_H */
