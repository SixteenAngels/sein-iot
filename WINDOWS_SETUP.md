# Windows Setup Guide for SEIN-IoT Firmware

This guide provides step-by-step instructions for building and flashing the SEIN-IoT firmware on Windows.

## Prerequisites

### System Requirements
- Windows 10 or later
- Python 3.8 or later
- Git for Windows
- 2 GB free disk space
- USB port for ESP32-S3 board

### Hardware
- ESP32-S3 development board
- USB-C or USB-B cable
- Lattice iCE40UP5K FPGA (optional for emulation)
- ATECC608A secure element (optional for emulation)

## Installation Steps

### 1. Install Python & Git

1. Download and install Python 3.8+ from https://www.python.org/
   - During installation, check "Add Python to PATH"
   - Check "pip" during installation

2. Download and install Git from https://git-scm.com/
   - Use default settings

### 2. Install ESP-IDF

#### Option A: Automated Installation (Recommended)
1. Download the ESP-IDF installer for Windows from:
   https://github.com/espressif/idf-installer/releases

2. Run the installer and follow the prompts

#### Option B: Manual Installation
1. Create a folder for ESP-IDF installation:
   ```
   C:\esp\esp-idf
   ```

2. Clone ESP-IDF repository:
   ```powershell
   cd C:\esp
   git clone --branch v5.0 https://github.com/espressif/esp-idf.git
   ```

3. Initialize ESP-IDF:
   ```powershell
   cd C:\esp\esp-idf
   .\install.bat esp32s3
   ```

4. Activate ESP-IDF environment:
   ```powershell
   export.bat
   # You should see: "Done! You can now run idf.py commands"
   ```

### 3. Install the SEIN-IoT Firmware

1. Clone or extract the SEIN-IoT firmware:
   ```powershell
   git clone <firmware-url>
   cd seiniot
   ```

2. Configure the build:
   ```powershell
   idf.py menuconfig
   ```
   - Go to "Serial flasher config"
   - Verify "Flash Mode" is set to "QIO"
   - Verify "Flash Frequency" is set to "80 MHz"
   - Save and exit

## Building the Firmware

### Option 1: Using build.bat (Recommended for Windows)

```powershell
cd seiniot

# Build the firmware
.\build.bat build

# Flash to board (replace COM3 with your port)
.\build.bat buildflash COM3

# Monitor serial output
.\build.bat monitor COM3
```

### Option 2: Using idf.py directly

```powershell
cd seiniot

# Build the firmware
idf.py build

# Flash to board (replace /dev/ttyUSB0 with your port)
idf.py -p COM3 flash

# Monitor serial output
idf.py -p COM3 monitor
```

## Finding Your Serial Port

### Method 1: Device Manager
1. Connect the ESP32-S3 board via USB
2. Open Device Manager (Win+X, then M)
3. Look for "COM" port under "Ports (COM & LPT)"
   - Usually appears as `COM3`, `COM4`, etc.
   - Note the port number

### Method 2: Command Line
```powershell
Get-WmiObject Win32_SerialPort | Select-Object Name, Description, DeviceID
```

### Method 3: Visual Studio Code
1. Open VS Code
2. Install "Serial Monitor" extension
3. Port will be shown in the status bar

## Troubleshooting

### Error: "Python not found"
- Add Python to PATH
- Restart command prompt
- Try: `python --version`

### Error: "idf.py not found"
- Ensure ESP-IDF export.bat was run
- Run again: `C:\esp\esp-idf\export.bat`
- Try in a new command prompt

### Error: "Port not found" during flash
- Check Device Manager for COM port
- Ensure USB cable is properly connected
- Try different USB port
- Restart board (press reset button)

### Error: "Unexpected timeout reading packet head"
- Press RESET button on board
- Hold BOOT button, then RESET, then release BOOT (programming mode)
- Try flash command again

### Build fails with "out of memory"
- Close unnecessary applications
- Increase virtual memory/page file
- Try building in a new command prompt session

## Configuration Options

### Change Log Level
```powershell
idf.py menuconfig
# Navigate to Component config → Log level → Default (Info)
```

### Change Flash Size
```powershell
idf.py menuconfig
# Navigate to Serial flasher config → Flash size → 16 MB
```

### Change CPU Frequency
```powershell
idf.py menuconfig
# Navigate to Component config → FreeRTOS → Tick rate
```

## Development Workflow

### 1. Edit Code
Edit files in:
- `src/` - Main application code
- `components/` - Modular components

### 2. Build
```powershell
.\build.bat build
```

### 3. Flash
```powershell
.\build.bat buildflash COM3
```

### 4. Monitor
```powershell
.\build.bat monitor COM3
```

### 5. Clean (if needed)
```powershell
.\build.bat clean
```

## VS Code Integration (Optional)

### Install Espressif IDF Extension
1. Open VS Code
2. Install extension: "ESP-IDF Extension"
3. Press Ctrl+Shift+P and select "ESP-IDF: Configure ESP-IDF Extension"
4. Follow setup wizard

### Build in VS Code
- Press Ctrl+Shift+B to build
- Use the status bar buttons for flash and monitor

## Next Steps

1. **Review the firmware**: Read [README.md](README.md)
2. **Understand the architecture**: See [PROJECT_STRUCTURE.md](PROJECT_STRUCTURE.md)
3. **Integrate hardware**: Follow [INTEGRATION_GUIDE.md](INTEGRATION_GUIDE.md)
4. **Check examples**: See [example_vibration_monitoring.c](example_vibration_monitoring.c)

## Support

For issues and questions:
- Check device documentation
- Review [BUILD.md](BUILD.md) for detailed build instructions
- Check [DEVELOPMENT.md](DEVELOPMENT.md) for coding guidelines

## Additional Resources

- ESP-IDF Documentation: https://docs.espressif.com/projects/esp-idf/
- ESP32-S3 Datasheet: https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf
- ESP32-S3 DevKit Documentation: https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/hw-reference/esp32s3/user-guide-devkitc-1.html
