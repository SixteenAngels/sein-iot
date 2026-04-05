# SEIN-IoT Firmware Documentation

## Overview

This is the firmware for the **Secure Edge Intelligence Node with IoT Integration (SEIN-IoT)**, a hybrid MCU-FPGA platform for trustworthy TinyML deployment on edge devices.

## Key Features

- **Hybrid Architecture**: ESP32-S3 MCU + Lattice iCE40UP5K FPGA + ATECC608A Secure Element
- **High Performance**: Sub-20ms inference latency, up to 56× DSP acceleration
- **Low Power**: <100mW during active operation, <10µA in deep sleep
- **Hardware Security**: Secure boot, authenticated OTA updates, TLS 1.3
- **Real-time Processing**: FreeRTOS with hard real-time guarantees
- **Edge ML**: TensorFlow Lite Micro with FPGA acceleration

## Firmware Architecture

```
┌─────────────────────────────────────────────────┐
│         Application Layer                       │
│  (TinyML Inference, IoT Connectivity, Control) │
└─────────────────────────────────────────────────┘
┌─────────────────────────────────────────────────┐
│         FreeRTOS Task Scheduler                  │
│  (8 prioritized tasks on 2 cores)               │
└─────────────────────────────────────────────────┘
┌──────────────┬─────────────┬──────────────┬──────┐
│   Security   │    FPGA     │   TinyML     │ WiFi │
│   Layer      │  Accel      │  Inference   │ BLE  │
│              │             │              │      │
└──────────────┴─────────────┴──────────────┴──────┘
┌──────────────┬─────────────┬──────────────┐
│  SPI         │  CRC16      │  Protocol    │
│  Protocol    │  Integrity  │  Framework   │
└──────────────┴─────────────┴──────────────┘
┌──────────────┬──────────────────┬─────────────┐
│     HAL      │     I2C (ATECC)  │     ADC     │
│   (GPIO)     │                  │   (Sensors) │
└──────────────┴──────────────────┴─────────────┘
┌──────────────────────────────────────────────────┐
│         Hardware Drivers (ESP-IDF)               │
│  (SPI, GPIO, I2C, ADC, WiFi, FreeRTOS, PSRAM)  │
└──────────────────────────────────────────────────┘
```

## Component Descriptions

### HAL (Hardware Abstraction Layer)
- **gpio_hal**: GPIO configuration and I/O
- **i2c_hal**: I2C master for ATECC608A communication
- **adc_hal**: Analog-to-digital converter for sensors
- **spi_hal**: SPI master interface (platform layer)

### Protocol Layer
- **crc16**: CRC-16-CCITT computation for SPI frame integrity
- **spi_protocol**: MCU-FPGA communication protocol with fragmentation support

### FPGA Acceleration
- **fpga_manager**: FPGA configuration and bitstream loading
- **fpga_dsp**: Accelerated DSP operations (FIR, FFT, MAC)

### Security
- **atecc608a_wrapper**: Cryptographic operations via secure element
- **crypto_ops**: High-level crypto primitives
- **secure_boot**: Firmware and bitstream signature verification

### TinyML
- **inference_engine**: TensorFlow Lite Micro inference wrapper

## Task Architecture

| Task Name | Core | Priority | Stack | Purpose |
|-----------|------|----------|-------|---------|
| sensor_task | 1 | 5 | 3KB | ADC sensor acquisition |
| fpga_accel_task | 1 | 6 | 4KB | FPGA command/response mgmt |
| inference_task | 1 | 4 | 4KB | ML model inference |
| display_task | 1 | 2 | 2KB | LCD update and UI |
| wifi_task | 0 | 3 | 8KB | WiFi connection |
| mqtt_task | 0 | 3 | 4KB | MQTT publish/subscribe |
| security_task | 0 | 7 | 3KB | Crypto operations |
| power_mon_task | 0 | 1 | 2KB | Battery monitoring |

## Pin Assignments

| Function | Pin | Direction | Purpose |
|----------|-----|-----------|---------|
| SPI MOSI | GPIO11 | ESP→FPGA | Data out to FPGA |
| SPI MISO | GPIO13 | FPGA→ESP | Data in from FPGA |
| SPI SCLK | GPIO12 | ESP→FPGA | SPI clock |
| SPI CS | GPIO10 | ESP→FPGA | Chip select |
| FPGA_READY | GPIO14 | FPGA→ESP | FPGA ready status |
| FPGA_IRQ | GPIO15 | FPGA→ESP | FPGA interrupt |
| FPGA_RESET | GPIO16 | ESP→FPGA | FPGA reset |
| I2C SDA | GPIO8 | Bidirectional | ATECC608A data |
| I2C SCL | GPIO9 | Bidirectional | ATECC608A clock |
| ADC0-6 | GPIO0-6 | Input | Sensor channels |

## Building and Flashing

See [BUILD.md](BUILD.md) for detailed build instructions.

Quick start:
```bash
cd seiniot
export IDF_PATH=/path/to/esp-idf
. $IDF_PATH/export.sh
idf.py build
idf.py flash
idf.py monitor
```

## Performance Benchmarks

### Computational Performance
| Operation | MCU Only | FPGA Accelerated | Speedup |
|-----------|----------|------------------|---------|
| 128-tap FIR | 45 ms | 0.8 ms | **56×** |
| 256-point FFT | 28 ms | 1.2 ms | **23×** |
| 512-point FFT | 61 ms | 2.8 ms | **22×** |
| 1024 MAC ops | 12 ms | 0.5 ms | **24×** |
| Dense layer | 8 ms | 1.8 ms | **4.4×** |
| Full pipeline | 85 ms | 18 ms | **4.7×** |

### Power Consumption
| Mode | Current | Power |
|------|---------|-------|
| Deep Sleep | 10 µA | - |
| Light Sleep | 800 µA | - |
| Idle | - | 15 mW |
| Processing | - | 85 mW |
| WiFi TX | - | 180 mW |

### Battery Life (3000 mAh Li-ion)
- **Continuous execution**: 130 hours (5.4 days)
- **10% duty cycle**: 1204 hours (50 days)
- **Typical IoT**: 1110 hours (46 days)

## Security Features

- **Secure Boot**: Signed bootloader and application images
- **Bitstream Authentication**: FPGA configuration verified before load
- **Cryptographic Operations**: ECDSA, SHA-256, AES via ATECC608A
- **OTA Updates**: Signed firmware with rollback protection
- **TLS 1.3**: Encrypted network communication
- **Hardware Key Storage**: Private keys in tamper-resistant element

## Configuration

Edit `include/config.h` to customize:
- Task stack sizes and priorities
- SPI protocol parameters
- FPGA configuration (FIR taps, FFT size)
- Power management settings
- Feature flags

## Serial Output

The firmware outputs extensive diagnostic information via serial (115200 baud):

```
I (40) APP_MAIN: SEIN-IoT Firmware v1.0.0
I (41) APP_MAIN: ================================================
I (48) APP_MAIN: Chip: ESP32-S3 (revision 0)
I (53) APP_MAIN: Features: WiFi BLE Flash PSRAM
I (58) APP_MAIN: Flash: 16 MB
...
I (200) FPGA_MANAGER: FPGA initialized successfully
I (300) TASKS: All tasks created successfully
I (310) APP_MAIN: Starting FreeRTOS scheduler...
```

## Debugging

Enable additional logging by setting log level in `sdkconfig.defaults`:
```
CONFIG_LOG_DEFAULT_LEVEL=4  # DEBUG level
```

Or at runtime via menuconfig:
```bash
idf.py menuconfig  # Component config → Logging
```

## Memory Usage

| Component | Size | Notes |
|-----------|------|-------|
| Firmware binary | ~1.5 MB | Including bootloader |
| SRAM used | 358 KB / 512 KB | 70% utilization |
| PSRAM used | 1.2 MB / 8 MB | 15% utilization |
| Flash models | 2-10 MB | Varies by model |
| Free heap | ~50 KB | Available for dynamic alloc |

## Future Enhancements

- Adaptive FPGA reconfiguration at runtime
- Federated learning support
- Matter protocol integration
- Post-quantum cryptography
- Multi-node mesh networking
- Energy harvesting support

## References

- [ESP32-S3 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf)
- [Lattice iCE40 Documentation](https://www.latticesemi.com/Products/FPGAandCPLD/iCE40)
- [ATECC608A Datasheet](https://www.microchip.com/en-us/product/atecc608a)
- [ESP-IDF Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/)
- [FreeRTOS Documentation](https://www.freertos.org/Documentation/161204_Mastering_the_FreeRTOS_Real_Time_Kernel-A_Hands-On_Tutorial_Guide.pdf)

## License

This firmware is released as open-source for research and production use.

## Support

For issues, questions, or contributions, please refer to the project repository.
