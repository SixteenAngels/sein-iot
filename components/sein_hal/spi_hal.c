/**
 * @file spi_hal.c
 * @brief SPI Hardware Abstraction Layer Implementation
 */

#include "spi_hal.h"
#include "esp_log.h"

static const char *TAG = "SPI_HAL";

bool spi_hal_init(uint32_t clock_speed_hz)
{
    /* SPI is initialized by spi_protocol_init() */
    ESP_LOGI(TAG, "SPI HAL initialized at %lu Hz", clock_speed_hz);
    return true;
}

bool spi_hal_write(const uint8_t *data, uint16_t len)
{
    /* Implemented at protocol layer */
    return true;
}

bool spi_hal_read(uint8_t *data, uint16_t len)
{
    /* Implemented at protocol layer */
    return true;
}

bool spi_hal_xfer(const uint8_t *tx_data, uint8_t *rx_data, uint16_t len)
{
    /* Implemented at protocol layer */
    return true;
}

bool spi_hal_deinit(void)
{
    /* Implemented at protocol layer */
    return true;
}
