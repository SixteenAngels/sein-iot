# FPGA Acceleration Component CMakeLists
idf_component_register(
    SRCS "fpga_manager.c"
         "fpga_dsp.c"
    INCLUDE_DIRS "."
    REQUIRES sein_hal protocol log freertos
)
