# HAL Component CMakeLists
idf_component_register(
    SRCS "gpio_hal.c"
         "i2c_hal.c"
         "adc_hal.c"
         "spi_hal.c"
    INCLUDE_DIRS "."
    REQUIRES driver esp_adc log freertos
)
