/**
 * @file sein_iot.h
 * @brief SEIN-IoT Platform Main Header
 * 
 * Secure Edge Intelligence Node with IoT Integration
 * Main system definitions and platform constants
 */

#ifndef SEIN_IOT_H
#define SEIN_IOT_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* ESP-IDF includes */
#include "esp_err.h"
#include "esp_log.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * Platform Configuration
 * ============================================================================ */

/* Hardware Versions */
#define SEIN_IOT_MAJOR      1
#define SEIN_IOT_MINOR      0
#define SEIN_IOT_PATCH      0

/* Device Configuration */
#define MCU_FREQ_MHZ        240         /* ESP32-S3 @ 240 MHz */
#define FPGA_FREQ_MHZ       24          /* iCE40UP5K nominal clock */
#define SPI_CLK_MHZ         10          /* SPI master clock (10 MHz nominal) */

/* Pin Assignments */
#define PIN_SPI_MOSI        11
#define PIN_SPI_MISO        13
#define PIN_SPI_SCLK        12
#define PIN_SPI_CS          10
#define PIN_FPGA_READY      14
#define PIN_FPGA_IRQ        15
#define PIN_FPGA_RESET      16

/* I2C Configuration (ATECC608A) */
#define PIN_I2C_SDA         8
#define PIN_I2C_SCL         9
#define ATECC608A_I2C_ADDR  0x60
#define I2C_CLK_KHZ         1000

/* ADC Configuration */
#define ADC_RESOLUTION_BITS 12
#define ADC_MAX_SAMPLES     256
#define ADC_SAMPLE_RATE_KHZ 10

/* Memory Boundaries */
#define SRAM_SIZE           (512 * 1024)        /* 512 KB internal SRAM */
#define PSRAM_SIZE          (8 * 1024 * 1024)   /* 8 MB external PSRAM */
#define FLASH_SIZE          (16 * 1024 * 1024)  /* 16 MB flash */

/* ============================================================================
 * Protocol Definitions
 * ============================================================================ */

/* SPI Protocol Command Codes */
typedef enum {
    SPI_CMD_PING        = 0x01,
    SPI_CMD_STATUS_READ = 0x02,
    SPI_CMD_FIR_RUN     = 0x10,
    SPI_CMD_FFT_RUN     = 0x11,
    SPI_CMD_MAC_LAYER   = 0x12,
    SPI_CMD_LOAD_WEIGHTS = 0x20,
    SPI_CMD_READ_WEIGHTS = 0x21,
    SPI_CMD_READ_RESULT = 0x30,
    SPI_CMD_RESET       = 0xF0,
    SPI_CMD_HALT        = 0xFF
} spi_cmd_t;

/* SPI Protocol Status Codes */
typedef enum {
    SPI_STATUS_OK       = 0x00,
    SPI_STATUS_ACK      = 0x55,
    SPI_STATUS_BUSY     = 0xEE,
    SPI_STATUS_ERROR    = 0xFF,
    SPI_STATUS_BAD_CRC  = 0x01,
    SPI_STATUS_INVALID_CMD = 0x02,
    SPI_STATUS_LEN_MISMATCH = 0x03,
    SPI_STATUS_MEM_OVERFLOW = 0x04,
    SPI_STATUS_AUTH_FAIL = 0x05
} spi_status_t;

/* ============================================================================
 * Operating Modes
 * ============================================================================ */

typedef enum {
    POWER_MODE_DEEP_SLEEP = 0,    /* 10 µA */
    POWER_MODE_LIGHT_SLEEP = 1,   /* 800 µA */
    POWER_MODE_IDLE = 2,          /* 15 mW */
    POWER_MODE_PROCESSING = 3,    /* 85 mW */
    POWER_MODE_WIFI_TX = 4        /* 180 mW */
} power_mode_t;

/* ============================================================================
 * Data Types
 * ============================================================================ */

/* Q15 Fixed-Point: [-1.0, +1.0) with 2^-15 resolution */
typedef int16_t q15_t;

/* Q7 Fixed-Point: [-1.0, +1.0) with 2^-7 resolution */
typedef int8_t q7_t;

/* Complex sample for FFT */
typedef struct {
    q15_t real;
    q15_t imag;
} complex_q15_t;

/* SPI Frame Structure */
typedef struct {
    uint8_t cmd;
    uint8_t len;
    uint8_t *data;
    uint16_t crc;
} spi_frame_t;

/* FPGA Status Register */
typedef struct {
    uint8_t fpga_ready : 1;
    uint8_t result_ready : 1;
    uint8_t error_flag : 1;
    uint8_t processing : 1;
    uint8_t reserved : 4;
} fpga_status_t;

/* ============================================================================
 * Function Declarations
 * ============================================================================ */

/**
 * Initialize SEIN-IoT platform
 * @return true if initialization successful, false otherwise
 */
bool sein_iot_init(void);

/**
 * Get platform status
 * @return Current power mode
 */
power_mode_t sein_iot_get_power_mode(void);

/**
 * Set platform power mode
 * @param mode Target power mode
 */
void sein_iot_set_power_mode(power_mode_t mode);

/**
 * Get FPGA status
 * @return FPGA status structure
 */
fpga_status_t sein_iot_get_fpga_status(void);

#ifdef __cplusplus
}
#endif

#endif /* SEIN_IOT_H */
