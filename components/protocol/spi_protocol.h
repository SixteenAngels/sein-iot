/**
 * @file spi_protocol.h
 * @brief SPI Protocol for ESP32-FPGA Communication
 * 
 * SPI Mode 0, 10 MHz nominal
 * CRC-16-CCITT integrity checking
 * Support for fragmented transfers
 */

#ifndef SPI_PROTOCOL_H
#define SPI_PROTOCOL_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "sein_iot.h"
#include "driver/spi_master.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * SPI Frame Format
 * ============================================================================ */

/*
 * Request Frame: [CMD | LEN | DATA (0-2048 bytes) | CRC_H | CRC_L]
 * Response Frame: [STATUS | LEN | RESP_DATA (0-2048 bytes) | CRC_H | CRC_L]
 */

typedef struct {
    uint8_t cmd;                    /* Command code */
    uint8_t len;                    /* Data length (0-255) */
    uint8_t data[512];              /* Data payload */
    uint16_t crc;                   /* CRC-16-CCITT (computed, not transmitted in data[]) */
} spi_frame_request_t;

typedef struct {
    uint8_t status;                 /* Status code */
    uint8_t len;                    /* Response data length (0-255) */
    uint8_t data[512];              /* Response data payload */
    uint16_t crc;                   /* CRC-16-CCITT */
} spi_frame_response_t;

/* ============================================================================
 * Fragmentation Context
 * ============================================================================ */

typedef struct {
    uint16_t fragment_idx;          /* Current fragment index */
    uint16_t total_fragments;       /* Total number of fragments */
    uint16_t bytes_received;        /* Bytes received so far */
    uint8_t state;                  /* Fragmentation state (0=idle, 1=in_progress) */
} fragmentation_context_t;

/* ============================================================================
 * SPI Protocol Interface
 * ============================================================================ */

/**
 * Initialize SPI protocol driver
 * @return true if successful, false otherwise
 */
bool spi_protocol_init(void);

/**
 * Deinitialize SPI protocol driver
 */
void spi_protocol_deinit(void);

/**
 * Send SPI command and receive response (single transaction)
 * 
 * @param cmd           Command code (spi_cmd_t)
 * @param cmd_data      Command data buffer (can be NULL if len=0)
 * @param cmd_len       Command data length
 * @param resp_data     Response data buffer (can be NULL if response not needed)
 * @param resp_len_max  Maximum response length
 * @param resp_len_out  Actual response length (output)
 * @return              Status code (0=OK, non-zero=error)
 */
uint8_t spi_protocol_transaction(
    uint8_t cmd,
    const uint8_t *cmd_data,
    uint8_t cmd_len,
    uint8_t *resp_data,
    uint16_t resp_len_max,
    uint16_t *resp_len_out
);

/**
 * Send fragmented data to FPGA
 * 
 * @param cmd           Command code
 * @param total_data    Pointer to complete data buffer
 * @param total_len     Total length of data
 * @param max_fragment_size  Max bytes per fragment
 * @return              true if successful, false otherwise
 */
bool spi_protocol_send_fragmented(
    uint8_t cmd,
    const uint8_t *total_data,
    uint32_t total_len,
    uint16_t max_fragment_size
);

/**
 * Read result from FPGA
 * 
 * @param result_data   Output buffer for result data
 * @param max_len       Maximum result length
 * @param actual_len    Actual result length (output)
 * @return              true if successful, false otherwise
 */
bool spi_protocol_read_result(
    uint8_t *result_data,
    uint16_t max_len,
    uint16_t *actual_len
);

/**
 * Check FPGA status
 * 
 * @param status_out    Pointer to status variable (output)
 * @return              true if successful, false otherwise
 */
bool spi_protocol_get_status(fpga_status_t *status_out);

/**
 * Reset FPGA
 * @return true if successful, false otherwise
 */
bool spi_protocol_reset_fpga(void);

/**
 * Core SPI write/read operation (low-level)
 * 
 * @param tx_data       TX buffer
 * @param rx_data       RX buffer
 * @param length        Transfer length
 * @return              true if successful, false otherwise
 */
bool spi_protocol_xfer(const uint8_t *tx_data, uint8_t *rx_data, uint16_t length);

#ifdef __cplusplus
}
#endif

#endif /* SPI_PROTOCOL_H */
