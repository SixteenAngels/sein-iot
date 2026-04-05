# Protocol Component CMakeLists
idf_component_register(
    SRCS "crc16.c"
         "spi_protocol.c"
    INCLUDE_DIRS "."
    REQUIRES driver log freertos sein_hal
)
