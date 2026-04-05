#!/bin/bash
# Build script for SEIN-IoT firmware
# Usage: ./build.sh [clean|build|flash|monitor|fullclean]

set -e

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${PROJECT_DIR}/build"
IDF_PATH="${IDF_PATH:-$HOME/esp/esp-idf}"

if [ ! -d "$IDF_PATH" ]; then
    echo "Error: ESP-IDF not found at $IDF_PATH"
    echo "Please install ESP-IDF or set IDF_PATH environment variable"
    exit 1
fi

cd "$PROJECT_DIR"

# Export ESP-IDF environment
source "$IDF_PATH/export.sh" > /dev/null 2>&1

case "${1:-build}" in
    clean)
        echo "Cleaning build artifacts..."
        rm -rf "$BUILD_DIR"
        echo "Clean complete"
        ;;
    
    build)
        echo "Building SEIN-IoT firmware..."
        idf.py build
        echo ""
        echo "Build complete!"
        idf.py size  # Show size breakdown
        ;;
    
    flash)
        if [ ! -f "$BUILD_DIR/sein_iot_firmware.bin" ]; then
            echo "Error: Firmware not built. Run './build.sh build' first"
            exit 1
        fi
        echo "Flashing firmware..."
        idf.py flash
        ;;
    
    monitor)
        echo "Opening serial monitor (Ctrl-C to exit)..."
        idf.py monitor
        ;;
    
    buildflash)
        $0 build
        $0 flash
        ;;
    
    fullclean)
        echo "Full clean (including configuration)..."
        rm -rf "$BUILD_DIR"
        rm -f sdkconfig
        echo "Full clean complete"
        ;;
    
    menuconfig)
        echo "Opening menuconfig..."
        idf.py menuconfig
        ;;
    
    *)
        echo "Usage: $0 {clean|build|flash|monitor|buildflash|fullclean|menuconfig}"
        exit 1
        ;;
esac
