/**
 * @file fpga_manager.c
 * @brief FPGA Management Implementation
 */

#include "fpga_manager.h"
#include "spi_protocol.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

static const char *TAG = "FPGA_MANAGER";

/* FPGA state tracking */
static bool fpga_initialized = false;
static uint16_t fpga_fir_taps = 0;
static uint16_t fpga_fft_size = 0;

bool fpga_manager_init(void)
{
    if (fpga_initialized) {
        return true;
    }

    /* Initialize SPI protocol */
    if (!spi_protocol_init()) {
        ESP_LOGE(TAG, "SPI protocol init failed");
        return false;
    }

    /* Wait for FPGA to be ready */
    vTaskDelay(pdMS_TO_TICKS(500));

    /* Ping FPGA */
    uint16_t resp_len;
    uint8_t status = spi_protocol_transaction(
        SPI_CMD_PING,
        NULL,
        0,
        NULL,
        0,
        &resp_len
    );

    if (status != SPI_STATUS_ACK && status != SPI_STATUS_OK) {
        ESP_LOGE(TAG, "FPGA not responding, status: 0x%02X", status);
        return false;
    }

    fpga_initialized = true;
    ESP_LOGI(TAG, "FPGA initialized successfully");
    return true;
}

void fpga_manager_deinit(void)
{
    if (fpga_initialized) {
        spi_protocol_deinit();
        fpga_initialized = false;
    }
}

bool fpga_load_bitstream(const uint8_t *bitstream_data, uint32_t bitstream_len)
{
    if (!fpga_initialized) {
        ESP_LOGE(TAG, "FPGA not initialized");
        return false;
    }

    if (!bitstream_data || bitstream_len == 0) {
        return false;
    }

    ESP_LOGI(TAG, "Loading bitstream (%lu bytes)", bitstream_len);

    /* Send bitstream via fragmented transfer */
    if (!spi_protocol_send_fragmented(
        SPI_CMD_LOAD_WEIGHTS,  /* Reuse for bitstream loading */
        bitstream_data,
        bitstream_len,
        256)) {
        ESP_LOGE(TAG, "Bitstream transfer failed");
        return false;
    }

    /* Wait for FPGA to configure */
    vTaskDelay(pdMS_TO_TICKS(100));

    /* Verify FPGA is ready */
    if (!fpga_is_ready()) {
        ESP_LOGE(TAG, "FPGA not ready after bitstream load");
        return false;
    }

    ESP_LOGI(TAG, "Bitstream loaded successfully");
    return true;
}

bool fpga_configure_fir(uint16_t num_taps, const q15_t *coefficients)
{
    if (!fpga_initialized) {
        return false;
    }

    if (num_taps == 0 || !coefficients) {
        return false;
    }

    /* Send FIR configuration to FPGA */
    uint8_t config_data[2];
    config_data[0] = (num_taps >> 8) & 0xFF;
    config_data[1] = num_taps & 0xFF;

    uint16_t resp_len;
    uint8_t status = spi_protocol_transaction(
        SPI_CMD_FIR_RUN,
        config_data,
        sizeof(config_data),
        NULL,
        0,
        &resp_len
    );

    if (status != SPI_STATUS_OK && status != SPI_STATUS_ACK) {
        ESP_LOGE(TAG, "FIR configuration failed, status: 0x%02X", status);
        return false;
    }

    /* Send filter coefficients */
    if (!spi_protocol_send_fragmented(
        SPI_CMD_LOAD_WEIGHTS,
        (const uint8_t *)coefficients,
        num_taps * sizeof(q15_t),
        256)) {
        ESP_LOGE(TAG, "FIR coefficients transfer failed");
        return false;
    }

    fpga_fir_taps = num_taps;
    ESP_LOGI(TAG, "FIR configured with %d taps", num_taps);
    return true;
}

bool fpga_configure_fft(uint16_t fft_size)
{
    if (!fpga_initialized) {
        return false;
    }

    if (fft_size == 0 || (fft_size & (fft_size - 1)) != 0) {
        ESP_LOGE(TAG, "Invalid FFT size: %d", fft_size);
        return false;
    }

    uint8_t size_data[2];
    size_data[0] = (fft_size >> 8) & 0xFF;
    size_data[1] = fft_size & 0xFF;

    uint16_t resp_len;
    uint8_t status = spi_protocol_transaction(
        SPI_CMD_FFT_RUN,
        size_data,
        sizeof(size_data),
        NULL,
        0,
        &resp_len
    );

    if (status != SPI_STATUS_OK && status != SPI_STATUS_ACK) {
        ESP_LOGE(TAG, "FFT configuration failed, status: 0x%02X", status);
        return false;
    }

    fpga_fft_size = fft_size;
    ESP_LOGI(TAG, "FFT configured for %d points", fft_size);
    return true;
}

bool fpga_configure_mac(uint16_t num_inputs, const q15_t *weights)
{
    if (!fpga_initialized || !weights || num_inputs == 0) {
        return false;
    }

    /* Send MAC configuration */
    uint8_t config_data[2];
    config_data[0] = (num_inputs >> 8) & 0xFF;
    config_data[1] = num_inputs & 0xFF;

    uint16_t resp_len;
    uint8_t status = spi_protocol_transaction(
        SPI_CMD_MAC_LAYER,
        config_data,
        sizeof(config_data),
        NULL,
        0,
        &resp_len
    );

    if (status != SPI_STATUS_OK && status != SPI_STATUS_ACK) {
        ESP_LOGE(TAG, "MAC configuration failed, status: 0x%02X", status);
        return false;
    }

    /* Load weights */
    if (!spi_protocol_send_fragmented(
        SPI_CMD_LOAD_WEIGHTS,
        (const uint8_t *)weights,
        num_inputs * sizeof(q15_t),
        256)) {
        ESP_LOGE(TAG, "MAC weights transfer failed");
        return false;
    }

    ESP_LOGI(TAG, "MAC configured for %d inputs", num_inputs);
    return true;
}

bool fpga_execute_fir(const q15_t *input_samples, uint16_t num_samples,
                      q15_t *output_samples, uint16_t *output_len)
{
    if (!fpga_initialized || !input_samples || !output_samples) {
        return false;
    }

    /* Send input samples */
    uint16_t resp_len;
    uint8_t status = spi_protocol_transaction(
        SPI_CMD_FIR_RUN,
        (const uint8_t *)input_samples,
        num_samples * sizeof(q15_t),
        NULL,
        0,
        &resp_len
    );

    if (status != SPI_STATUS_OK && status != SPI_STATUS_ACK && status != SPI_STATUS_BUSY) {
        ESP_LOGE(TAG, "FIR execution send failed, status: 0x%02X", status);
        return false;
    }

    /* If BUSY, poll for results */
    if (status == SPI_STATUS_BUSY) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    /* Read results */
    if (!spi_protocol_read_result((uint8_t *)output_samples, 512, &resp_len)) {
        ESP_LOGE(TAG, "FIR result read failed");
        return false;
    }

    *output_len = resp_len / sizeof(q15_t);
    return true;
}

bool fpga_execute_fft(const q15_t *input_samples, uint16_t num_samples,
                      complex_q15_t *output_spectrum, uint16_t *output_len)
{
    if (!fpga_initialized || !input_samples || !output_spectrum) {
        return false;
    }

    /* Send input samples */
    uint16_t resp_len;
    uint8_t status = spi_protocol_transaction(
        SPI_CMD_FFT_RUN,
        (const uint8_t *)input_samples,
        num_samples * sizeof(q15_t),
        NULL,
        0,
        &resp_len
    );

    if (status == SPI_STATUS_BUSY) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    /* Read results */
    if (!spi_protocol_read_result((uint8_t *)output_spectrum, 512, &resp_len)) {
        ESP_LOGE(TAG, "FFT result read failed");
        return false;
    }

    *output_len = resp_len / sizeof(complex_q15_t);
    return true;
}

bool fpga_execute_mac(const q15_t *inputs, uint16_t num_inputs, q15_t *output)
{
    if (!fpga_initialized || !inputs || !output) {
        return false;
    }

    uint16_t resp_len;
    uint8_t status = spi_protocol_transaction(
        SPI_CMD_MAC_LAYER,
        (const uint8_t *)inputs,
        num_inputs * sizeof(q15_t),
        (uint8_t *)output,
        sizeof(q15_t),
        &resp_len
    );

    return (status == SPI_STATUS_OK && resp_len == sizeof(q15_t));
}

bool fpga_is_ready(void)
{
    if (!fpga_initialized) {
        return false;
    }

    fpga_status_t status = fpga_get_status();
    return status.fpga_ready;
}

fpga_status_t fpga_get_status(void)
{
    fpga_status_t status = {0};

    if (fpga_initialized) {
        spi_protocol_get_status(&status);
    }

    return status;
}
