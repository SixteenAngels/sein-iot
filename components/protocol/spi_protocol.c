/**
 * @file spi_protocol.c
 * @brief SPI Protocol Implementation
 */

#include <string.h>
#include "spi_protocol.h"
#include "crc16.h"
#include "esp_log.h"
#include "esp_err.h"

static const char *TAG = "SPI_PROTOCOL";

/* SPI Device Handle */
static spi_device_handle_t spi_device = NULL;

/* Fragmentation context */
static fragmentation_context_t frag_context = {0};

/**
 * Initialize SPI protocol driver
 */
bool spi_protocol_init(void)
{
    if (spi_device != NULL) {
        ESP_LOGW(TAG, "SPI protocol already initialized");
        return true;
    }

    /* Initialize SPI bus */
    spi_bus_config_t bus_config = {
        .mosi_io_num = 11,      /* GPIO11 */
        .miso_io_num = 13,      /* GPIO13 */
        .sclk_io_num = 12,      /* GPIO12 */
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 2048 + 8,
        .flags = 0,
    };

    esp_err_t ret = spi_bus_initialize(SPI2_HOST, &bus_config, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPI bus init failed: %d", ret);
        return false;
    }

    /* Attach device to SPI bus */
    spi_device_interface_config_t dev_config = {
        .mode = 0,                          /* SPI Mode 0 */
        .clock_speed_hz = 10 * 1000 * 1000, /* 10 MHz */
        .spics_io_num = 10,                 /* GPIO10 for CS */
        .queue_size = 7,
        .cs_ena_pretrans = 0,
        .cs_ena_posttrans = 0,
        .flags = SPI_DEVICE_NO_DUMMY,
    };

    ret = spi_bus_add_device(SPI2_HOST, &dev_config, &spi_device);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPI device add failed: %d", ret);
        spi_bus_free(SPI2_HOST);
        return false;
    }

    /* Configure control pins */
    gpio_config_t gpio_cfg = {
        .pin_bit_mask = (1ULL << 14) | (1ULL << 15) | (1ULL << 16),
        .mode = GPIO_MODE_INPUT_OUTPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    ret = gpio_config(&gpio_cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "GPIO config failed: %d", ret);
        return false;
    }

    memset(&frag_context, 0, sizeof(frag_context));

    ESP_LOGI(TAG, "SPI protocol initialized successfully");
    return true;
}

/**
 * Deinitialize SPI protocol driver
 */
void spi_protocol_deinit(void)
{
    if (spi_device != NULL) {
        spi_bus_remove_device(spi_device);
        spi_spi_bus_free(SPI2_HOST);
        spi_device = NULL;
    }
    ESP_LOGI(TAG, "SPI protocol deinitialized");
}

/**
 * Core SPI write/read operation (low-level)
 */
bool spi_protocol_xfer(const uint8_t *tx_data, uint8_t *rx_data, uint16_t length)
{
    if (spi_device == NULL) {
        ESP_LOGE(TAG, "SPI device not initialized");
        return false;
    }

    spi_transaction_t trans = {
        .length = length * 8,
        .tx_buffer = tx_data,
        .rx_buffer = rx_data,
    };

    esp_err_t ret = spi_device_polling_transmit(spi_device, &trans);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPI transfer failed: %d", ret);
        return false;
    }

    return true;
}

/**
 * Send SPI command and receive response (single transaction)
 */
uint8_t spi_protocol_transaction(
    uint8_t cmd,
    const uint8_t *cmd_data,
    uint8_t cmd_len,
    uint8_t *resp_data,
    uint16_t resp_len_max,
    uint16_t *resp_len_out)
{
    if (spi_device == NULL) {
        return SPI_STATUS_ERROR;
    }

    uint8_t tx_buffer[512 + 8] = {0};
    uint8_t rx_buffer[512 + 8] = {0};
    uint16_t frame_len;

    /* Build request frame */
    tx_buffer[0] = cmd;
    tx_buffer[1] = cmd_len;

    if (cmd_data && cmd_len > 0) {
        memcpy(&tx_buffer[2], cmd_data, cmd_len);
    }

    /* Compute CRC over CMD | LEN | DATA */
    uint16_t crc = crc16_compute(tx_buffer, 2 + cmd_len);
    tx_buffer[2 + cmd_len] = (crc >> 8) & 0xFF;      /* CRC high byte */
    tx_buffer[3 + cmd_len] = crc & 0xFF;             /* CRC low byte */

    frame_len = 4 + cmd_len;  /* CMD + LEN + DATA + CRC */

    /* Transfer */
    if (!spi_protocol_xfer(tx_buffer, rx_buffer, frame_len)) {
        return SPI_STATUS_ERROR;
    }

    /* Parse response */
    uint8_t status = rx_buffer[0];
    uint8_t resp_len = rx_buffer[1];

    if (resp_len > resp_len_max) {
        ESP_LOGW(TAG, "Response length overflow: %d > %d", resp_len, resp_len_max);
        return SPI_STATUS_LEN_MISMATCH;
    }

    /* Verify response CRC */
    uint16_t resp_crc = ((uint16_t)rx_buffer[2 + resp_len] << 8) | rx_buffer[3 + resp_len];
    if (!crc16_verify(rx_buffer, 2 + resp_len, resp_crc)) {
        ESP_LOGW(TAG, "Response CRC mismatch");
        return SPI_STATUS_BAD_CRC;
    }

    /* Copy response data if requested */
    if (resp_data && resp_len > 0) {
        memcpy(resp_data, &rx_buffer[2], resp_len);
    }

    if (resp_len_out) {
        *resp_len_out = resp_len;
    }

    return status;
}

/**
 * Send fragmented data to FPGA
 */
bool spi_protocol_send_fragmented(
    uint8_t cmd,
    const uint8_t *total_data,
    uint32_t total_len,
    uint16_t max_fragment_size)
{
    if (!total_data || total_len == 0) {
        return false;
    }

    uint32_t bytes_sent = 0;
    uint16_t fragment_idx = 0;

    while (bytes_sent < total_len) {
        uint16_t chunk_size = (total_len - bytes_sent) > max_fragment_size ?
                              max_fragment_size : (total_len - bytes_sent);

        /* Build fragment header (2 bytes for fragment index) */
        uint8_t frag_header[2];
        frag_header[0] = (fragment_idx >> 8) & 0xFF;
        frag_header[1] = fragment_idx & 0xFF;

        /* Transfer fragment */
        uint16_t resp_len;
        uint8_t status = spi_protocol_transaction(
            cmd,
            &total_data[bytes_sent],
            chunk_size,
            NULL,
            0,
            &resp_len
        );

        if (status != SPI_STATUS_OK && status != SPI_STATUS_ACK) {
            ESP_LOGE(TAG, "Fragment %d send failed, status: 0x%02X", fragment_idx, status);
            return false;
        }

        bytes_sent += chunk_size;
        fragment_idx++;
    }

    ESP_LOGI(TAG, "Fragmented transfer complete: %lu bytes in %d fragments",
             total_len, fragment_idx);
    return true;
}

/**
 * Read result from FPGA
 */
bool spi_protocol_read_result(
    uint8_t *result_data,
    uint16_t max_len,
    uint16_t *actual_len)
{
    uint16_t resp_len;
    uint8_t status = spi_protocol_transaction(
        SPI_CMD_READ_RESULT,
        NULL,
        0,
        result_data,
        max_len,
        &resp_len
    );

    if (status != SPI_STATUS_OK) {
        ESP_LOGW(TAG, "Read result failed, status: 0x%02X", status);
        return false;
    }

    if (actual_len) {
        *actual_len = resp_len;
    }

    return true;
}

/**
 * Check FPGA status
 */
bool spi_protocol_get_status(fpga_status_t *status_out)
{
    uint8_t status_byte;
    uint16_t resp_len;

    uint8_t status = spi_protocol_transaction(
        SPI_CMD_STATUS_READ,
        NULL,
        0,
        &status_byte,
        1,
        &resp_len
    );

    if (status != SPI_STATUS_OK) {
        ESP_LOGW(TAG, "Status read failed, status: 0x%02X", status);
        return false;
    }

    if (status_out) {
        status_out->fpga_ready = (status_byte >> 0) & 1;
        status_out->result_ready = (status_byte >> 1) & 1;
        status_out->error_flag = (status_byte >> 2) & 1;
        status_out->processing = (status_byte >> 3) & 1;
        status_out->reserved = (status_byte >> 4) & 0x0F;
    }

    return true;
}

/**
 * Reset FPGA
 */
bool spi_protocol_reset_fpga(void)
{
    uint16_t resp_len;
    uint8_t status = spi_protocol_transaction(
        SPI_CMD_RESET,
        NULL,
        0,
        NULL,
        0,
        &resp_len
    );

    return (status == SPI_STATUS_OK);
}
