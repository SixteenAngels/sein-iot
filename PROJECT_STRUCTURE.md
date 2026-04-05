seiniot/
├── CMakeLists.txt                    # Root CMake configuration
├── README.md                         # Project documentation
├── BUILD.md                          # Detailed build instructions
├── QUICKSTART.md                     # Quick start guide
├── sdkconfig.defaults               # ESP-IDF default configuration
├── partitions.csv                   # Flash partition table
├── idf_component.yml                # Component dependencies
│
├── src/                             # Main application source
│   ├── main.c                       # Entry point
│   └── freertos_tasks.c             # FreeRTOS task implementations
│
├── include/                         # Application headers
│   ├── sein_iot.h                   # Platform definitions
│   ├── config.h                     # Compile-time configuration
│   └── freertos_tasks.h             # Task declarations
│
├── components/
│   ├── hal/                         # Hardware Abstraction Layer
│   │   ├── CMakeLists.txt
│   │   ├── gpio_hal.h               # GPIO interface
│   │   ├── gpio_hal.c
│   │   ├── i2c_hal.h                # I2C interface
│   │   ├── i2c_hal.c
│   │   ├── adc_hal.h                # ADC interface
│   │   ├── adc_hal.c
│   │   ├── spi_hal.h                # SPI interface
│   │   └── spi_hal.c
│   │
│   ├── protocol/                    # SPI Protocol Layer
│   │   ├── CMakeLists.txt
│   │   ├── crc16.h                  # CRC-16-CCITT implementation
│   │   ├── crc16.c
│   │   ├── spi_protocol.h           # MCU-FPGA SPI protocol
│   │   └── spi_protocol.c
│   │
│   ├── fpga_accel/                  # FPGA Acceleration
│   │   ├── CMakeLists.txt
│   │   ├── fpga_manager.h           # FPGA control and config
│   │   ├── fpga_manager.c
│   │   ├── fpga_dsp.h               # DSP algorithms (FIR, FFT)
│   │   └── fpga_dsp.c
│   │
│   ├── security/                    # Security Layer
│   │   ├── CMakeLists.txt
│   │   ├── atecc608a_wrapper.h      # Secure element interface
│   │   ├── atecc608a_wrapper.c
│   │   ├── crypto_ops.h             # High-level crypto
│   │   ├── crypto_ops.c
│   │   ├── secure_boot.h            # Secure boot verification
│   │   └── secure_boot.c
│   │
│   └── tinyml/                      # TinyML Inference
│       ├── CMakeLists.txt
│       ├── inference_engine.h       # TFLite Micro wrapper
│       └── inference_engine.c
│
├── fpga/                            # FPGA bitstreams and HDL
│   └── (Verilog/VHDL sources)
│
├── models/                          # TinyML models
│   └── (TFLite .bin files)
│
└── build/                           # Build output (auto-generated)
    ├── sein_iot_firmware.elf
    ├── sein_iot_firmware.bin
    ├── bootloader.bin
    └── partition_table.bin


## File Statistics

### Source Code
- Drivers (HAL):       ~400 lines
- Protocol Layer:      ~500 lines
- FPGA/DSP:           ~400 lines
- Security:           ~500 lines
- TinyML:             ~200 lines
- Tasks:              ~350 lines
- Total:              ~2,350 lines

### Component Breakdown
- Hardware drivers:    40% (GPIO, I2C, ADC, SPI)
- Protocol/Comm:       20% (CRC, SPI framing)
- Acceleration:        15% (FPGA, DSP)
- Security:            20% (Crypto, secure boot)
- ML:                  5% (Inference wrapper)

### Binary Sizes (typical)
- Bootloader:        16 KB
- Application:        ~1.5 MB
- FPGA bitstream:    ~1.2 MB
- Models (optional):  1-10 MB
- Total flash:      <16 MB available

### Memory Usage
- Stack (all tasks):  ~80 KB total
- Heap (active):      ~50 KB
- PSRAM:             1.2 MB available
- SRAM:              358 KB used / 512 KB total
