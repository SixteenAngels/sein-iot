# SEIN-IoT Quick Start Guide

## Getting Started in 5 Minutes

### 1. Prerequisites

```bash
# Install ESP-IDF (if not already installed)
git clone --recursive https://github.com/espressif/esp-idf.git
cd esp-idf
git checkout release/v5.1
./install.sh
```

### 2. Setup Environment

```bash
cd /path/to/seiniot

# On Linux/Mac
source $HOME/esp/esp-idf/export.sh

# On Windows PowerShell
& "$Env:USERPROFILE\esp\esp-idf\export.ps1"
```

### 3. Configure for Your Board

```bash
idf.py menuconfig
```

Navigate to:
- `Target device` → Select `esp32s3`
- `Serial flasher config` → Set your COM port
- `Component config` → `FreeRTOS` → Review task settings

Press `S` to save and `Q` to quit.

### 4. Build the Firmware

```bash
idf.py build
```

Expected output:
```
...
[100%] Built target app_process_elf
Generated firmware size: ...
```

### 5. Flash to Hardware

```bash
idf.py flash
```

### 6. Monitor Serial Output

```bash
idf.py monitor
```

You should see:
```
SEIN-IoT Firmware v1.0.0
================================================
Chip: ESP32-S3 (revision 0)
Features: WiFi BLE Flash PSRAM
FPGA manager initialized successfully
All tasks created successfully
```

### 7. Verify Hardware Connectivity

Check that these appear in the logs:
- ✅ NVS initialized
- ✅ GPIO HAL initialized
- ✅ ADC HAL initialized
- ✅ Secure boot initialized
- ✅ FPGA manager initialized
- ✅ All tasks created

## Customization

### Change Log Level

Edit `sdkconfig.defaults` or use menuconfig:
```
CONFIG_LOG_DEFAULT_LEVEL=4  # 0=NONE, 1=ERROR, 2=WARN, 3=INFO, 4=DEBUG, 5=VERBOSE
```

### Adjust Task Parameters

Edit `include/config.h`:
```c
#define PRIORITY_SENSOR_TASK        5  /* 0-255, higher = higher priority */
#define STACK_SIZE_SENSOR_TASK      3072  /* Bytes */
```

### Enable/Disable Features

In `include/config.h`:
```c
#define ENABLE_WIFI                 1
#define ENABLE_BLE                  1
#define ENABLE_SECURE_BOOT          1
#define ENABLE_FPGA_ACCELERATION    1
```

## Common Issues

### "idf.py: command not found"
```bash
# Make sure ESP-IDF is in your PATH
source $HOME/esp/esp-idf/export.sh
```

### "FPGA not responding"
- Check SPI pin connections (GPIO 10, 11, 12, 13)
- Verify FPGA bitstream is burned
- Try `idf.py erase_flash && idf.py flash`

### High memory usage
- Reduce STACK_SIZE_* values in config.h (minimum 1024 bytes)
- Disable unused features (WiFi, BLE, etc.)
- Check `idf.py size` output

### Serial port permission denied (Linux)
```bash
sudo usermod -a -G dialout $USER
# Log out and back in
```

## Next Steps

1. **Generate cryptographic keys** for secure boot
2. **Create/train TinyML model** and add to `models/`
3. **Design FPGA bitstream** for your accelerators
4. **Configure sensor interfaces** in HAL
5. **Implement WiFi/MQTT connectivity**
6. **Test OTA update mechanism**

## Build Output Files

Located in `build/` directory:
- `sein_iot_firmware.elf` - Executable with debug symbols
- `sein_iot_firmware.bin` - Binary for flashing
- `bootloader.bin` - Secure bootloader
- `partition_table.bin` - Flash layout

## Performance Testing

After building, check firmware metrics:

```bash
idf.py size                    # Binary size analysis
idf.py partition_table         # Flash layout
idf.py efuse-summary           # Security status (if eFuses burned)
```

## Full Documentation

See these files for more details:
- [README.md](README.md) - Architecture and features
- [BUILD.md](BUILD.md) - Detailed build instructions
- [include/config.h](include/config.h) - All configuration options

## Support

For questions or issues:
1. Check the serial output for error messages
2. Enable debug logging with `CONFIG_LOG_DEFAULT_LEVEL=5`
3. Review component logs in `components/*/CMakeLists.txt`
4. Check ESP-IDF documentation: https://docs.espressif.com/

Good luck with your SEIN-IoT build! 🚀
