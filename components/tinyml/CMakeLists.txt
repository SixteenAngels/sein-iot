# TinyML Component CMakeLists
idf_component_register(
    SRCS "inference_engine.c"
    INCLUDE_DIRS "."
    REQUIRES sein_hal freertos log
)
