# Security Component CMakeLists
idf_component_register(
    SRCS "atecc608a_wrapper.c"
         "crypto_ops.c"
         "secure_boot.c"
    INCLUDE_DIRS "."
    REQUIRES sein_hal mbedtls log freertos
)