/**
 * @file config.h
 * @brief Compile-time configuration for SEIN-IoT firmware
 */

#ifndef CONFIG_H
#define CONFIG_H

/* ============================================================================
 * Build Configuration
 * ============================================================================ */

#define DEBUG_ENABLE                1
#define DEBUG_UART_BAUDRATE         115200

/* ============================================================================
 * FreeRTOS Configuration
 * ============================================================================ */

#define FREERTOS_STACK_DEPTH_WORDS  4096
#define FREERTOS_TICK_RATE_HZ       1000

/* Task Stack Sizes (words) */
#define STACK_SIZE_SENSOR_TASK      3072
#define STACK_SIZE_FPGA_ACCEL_TASK  4096
#define STACK_SIZE_INFERENCE_TASK   4096
#define STACK_SIZE_WIFI_TASK        8192
#define STACK_SIZE_MQTT_TASK        4096
#define STACK_SIZE_SECURITY_TASK    3072
#define STACK_SIZE_DISPLAY_TASK     2048
#define STACK_SIZE_POWER_MON_TASK   2048

/* Task Priorities */
#define PRIORITY_POWER_MON_TASK     1
#define PRIORITY_DISPLAY_TASK       2
#define PRIORITY_INFERENCE_TASK     4
#define PRIORITY_WIFI_TASK          3
#define PRIORITY_MQTT_TASK          3
#define PRIORITY_SENSOR_TASK        5
#define PRIORITY_FPGA_ACCEL_TASK    6
#define PRIORITY_SECURITY_TASK      7

/* ============================================================================
 * SPI Protocol Configuration
 * ============================================================================ */

#define SPI_MAX_PAYLOAD_BYTES       512
#define SPI_TX_DMA_CHAN             SPI_DMA_CH_AUTO
#define SPI_RX_DMA_CHAN             SPI_DMA_CH_AUTO
#define SPI_TIMEOUT_MS              1000

/* ============================================================================
 * FPGA Configuration
 * ============================================================================ */

#define FPGA_CONFIG_TIMEOUT_MS      5000
#define FPGA_MAX_BLOCK_RAM_BYTES    (120 * 1024 / 8)  /* 120 Kbit = 15 KB */

/* FIR Filter Configuration */
#define FIR_MAX_TAPS                256
#define FIR_DEFAULT_TAPS            64

/* FFT Configuration */
#define FFT_MAX_POINTS              1024
#define FFT_DEFAULT_POINTS          256

/* MAC Operation Configuration */
#define MAC_MAX_INPUTS              128
#define MAC_PARALLEL_UNITS          8

/* ============================================================================
 * Security Configuration
 * ============================================================================ */

#define ATECC608A_RETRY_COUNT       3
#define ATECC608A_TIMEOUT_MS        500

/* Key Slot Allocation */
#define KEY_SLOT_DEVICE_IDENTITY    0
#define KEY_SLOT_TLS_CERT           4
#define KEY_SLOT_BITSTREAM_SIGN     8
#define KEY_SLOT_APP_KEY_0          12
#define KEY_SLOT_APP_KEY_1          13

/* ============================================================================
 * TinyML Configuration
 * ============================================================================ */

#define TINYML_MAX_MODEL_SIZE_BYTES  (2 * 1024 * 1024)  /* 2 MB */
#define TINYML_TENSOR_ARENA_SIZE     (256 * 1024)       /* 256 KB */
#define TINYML_MAX_LAYERS            20

/* ============================================================================
 * Power Management Configuration
 * ============================================================================ */

#define BATTERY_VOLTAGE_THRESHOLD   3.0  /* Volts - shutdown threshold */
#define BATTERY_SAMPLE_INTERVAL_MS  60000 /* 60 seconds */

/* ============================================================================
 * Network Configuration
 * ============================================================================ */

#define WIFI_SSID                   "SEIN-IoT-Network"
#define WIFI_PASSWORD               "SecureEdgeNode2024"
#define MQTT_BROKER_URL             "mqtt://broker.example.com"
#define MQTT_BROKER_PORT            1883

/* ============================================================================
 * Sensor Configuration
 * ============================================================================ */

#define SENSOR_I2C_TIMEOUT_MS       100
#define SENSOR_MAX_DEVICES          8

/* ============================================================================
 * Feature Flags
 * ============================================================================ */

#define ENABLE_WIFI                 1
#define ENABLE_BLE                  1
#define ENABLE_OTA                  1
#define ENABLE_SECURE_BOOT          1
#define ENABLE_FPGA_ACCELERATION    1
#define ENABLE_TINYML               1
#define ENABLE_POWER_MANAGEMENT     1

#endif /* CONFIG_H */
