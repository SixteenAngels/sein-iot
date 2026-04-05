# SEIN-IoT Build Instructions

## Prerequisites

- **ESP-IDF**: v5.0 or later
  ```bash
  git clone --recursive https://github.com/espressif/esp-idf.git
  cd esp-idf
  ./install.sh
  . ./export.sh
  ```

- **Build Tools**:
  - Python 3.8+
  - GCC toolchain (included with ESP-IDF)
  - CMake 3.16+

- **Hardware**:
  - ESP32-S3 WROOM module
  - Lattice iCE40UP5K FPGA
  - ATECC608A secure element
  - USB-C cable for programming

## Building the Firmware

### 1. Prepare the Build Environment

```bash
cd /path/to/seiniot
. $IDF_PATH/export.sh
```

### 2. Configure the Build

```bash
idf.py menuconfig
```

Key settings to configure:
- Target device: ESP32-S3
- Flash size: 16 MB
- Flash mode: QIO @ 80 MHz
- PSRAM: Enabled
- CPU frequency: 240 MHz

### 3. Build the Firmware

```bash
idf.py build
```

This will:
- Compile all C/C++ source files
- Link the ELF executable
- Generate binary image files
- Create the final firmware package

### 4. Flash the Firmware

```bash
idf.py flash
```

Or manually:
```bash
esptool.py -p /dev/ttyUSB0 write_flash 0x0 build/firmware.bin
```

### 5. Monitor Serial Output

```bash
idf.py monitor
```

This opens a serial port terminal showing ESP32 logs.

## Build Targets

- `idf.py build` - Build firmware
- `idf.py clean` - Clean build artifacts
- `idf.py fullclean` - Clean everything including config
- `idf.py size` - Show binary size analysis
- `idf.py partition_table` - Generate partition table
- `idf.py efuse-summary` - Read eFuse summary

## Building Components Individually

```bash
# Build only HAL
idf.py build -C components/hal

# Build only FPGA acceleration
idf.py build -C components/fpga_accel
```

## Creating a Release Binary

```bash
idf.py build
cp build/firmware.bin releases/sein_iot_v1.0.bin
cp build/bootloader/bootloader.bin releases/bootloader_v1.0.bin
```

## Debugging

Enable debug logging:
```bash
idf.py build -DCMAKE_BUILD_TYPE=Debug
idf.py monitor -p /dev/ttyUSB0 --baudrate 115200
```

## OTA (Over-The-Air) Update

```bash
# Generate OTA image
idf.py build
cp build/firmware.bin sein_iot_ota.bin

# Sign the image (if secure boot enabled)
esptool.py sign_data -k secure_boot_key.pem sein_iot_ota.bin
```

## Troubleshooting

### CMake errors
```bash
rm -rf build/
idf.py build
```

### Flash verification failed
```bash
idf.py erase_flash
idf.py flash
```

### Serial port permission denied
```bash
# Linux: Add user to dialout group
sudo usermod -a -G dialout $USER
# Logout and log back in

# Or use sudo
sudo idf.py flash
```

## Performance Validation

After flashing, run the following to verify:

1. **System startup** (observe serial logs)
   - Secure boot verification
   - FPGA bitstream loading
   - Task creation

2. **FPGA communication test**
   - Monitor task should show FPGA ready status
   - SPI protocol should report successful status reads

3. **Sensor acquisition** (with sensors connected)
   - ADC readings in serial output
   - Timing: ~100ms per sample batch

4. **Inference** (with TinyML model loaded)
   - Inference latency: <20ms target
   - Output classification on serial logs

## Release Build Checklist

- [ ] Build succeeds with zero warnings
- [ ] Binary size is within limits
- [ ] All security features verified
- [ ] OTA image signed and tested
- [ ] Performance benchmarks pass
- [ ] Documentation updated
