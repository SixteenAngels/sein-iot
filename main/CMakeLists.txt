# Main Component CMakeLists
idf_component_register(
    SRCS "main.c"
         "freertos_tasks.c"
    INCLUDE_DIRS "../include"
    REQUIRES
        esp_wifi
        esp_http_client
        mbedtls
        esp_adc
        spiffs
        sein_hal
        protocol
        fpga_accel
        security
        tinyml
)
