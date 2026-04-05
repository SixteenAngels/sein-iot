@echo off
REM Build script for SEIN-IoT Firmware on Windows
REM Usage: build.bat [clean|build|flash|monitor|fullclean|buildflash]

setlocal enabledelayedexpansion

if "%1"=="" goto usage

REM Check if ESP-IDF is installed
if not defined IDF_PATH (
    echo Error: IDF_PATH environment variable not set
    echo Please set up ESP-IDF first
    exit /b 1
)

REM Get the script directory
for %%I in ("%~dp0.") do set SCRIPT_DIR=%%~fI

cd /d "%SCRIPT_DIR%"

if "%1"=="clean" (
    echo Cleaning build directory...
    if exist build rmdir /s /q build
    del /f /q sdkconfig
    del /f /q sdkconfig.old
    echo Clean complete
    exit /b 0
)

if "%1"=="build" (
    echo Building firmware...
    idf.py build
    exit /b !errorlevel!
)

if "%1"=="flash" (
    if "%2"=="" (
        echo Error: Port not specified. Usage: build.bat flash COM3
        exit /b 1
    )
    echo Flashing firmware to port %2...
    idf.py -p %2 flash
    exit /b !errorlevel!
)

if "%1"=="monitor" (
    if "%2"=="" (
        echo Error: Port not specified. Usage: build.bat monitor COM3
        exit /b 1
    )
    echo Monitoring serial output on port %2...
    idf.py -p %2 monitor
    exit /b !errorlevel!
)

if "%1"=="menuconfig" (
    echo Opening menuconfig...
    idf.py menuconfig
    exit /b !errorlevel!
)

if "%1"=="buildflash" (
    if "%2"=="" (
        echo Error: Port not specified. Usage: build.bat buildflash COM3
        exit /b 1
    )
    echo Building and flashing to port %2...
    idf.py -p %2 build flash
    exit /b !errorlevel!
)

if "%1"=="fullclean" (
    echo Performing full clean...
    if exist build rmdir /s /q build
    del /f /q sdkconfig
    del /f /q sdkconfig.old
    if exist .vscode\settings.json del /f /q .vscode\settings.json
    echo Full clean complete
    exit /b 0
)

if "%1"=="help" (
    echo SEIN-IoT Firmware Build Script
    echo.
    echo Usage: build.bat COMMAND [ARGS]
    echo.
    echo Commands:
    echo   clean              Clean build artifacts
    echo   build              Build firmware
    echo   flash PORT         Flash firmware to serial port (e.g., COM3)
    echo   monitor PORT       Monitor serial output (e.g., COM3)
    echo   menuconfig         Open configuration menu
    echo   buildflash PORT    Build and flash in one step
    echo   fullclean          Complete cleanup including IDE settings
    echo   help               Show this help message
    exit /b 0
)

:usage
echo SEIN-IoT Firmware Build Script
echo.
echo Usage: build.bat COMMAND [ARGS]
echo.
echo Commands:
echo   clean              Clean build artifacts
echo   build              Build firmware
echo   flash PORT         Flash firmware to serial port (e.g., COM3)
echo   monitor PORT       Monitor serial output (e.g., COM3)
echo   menuconfig         Open configuration menu
echo   buildflash PORT    Build and flash in one step
echo   fullclean          Complete cleanup including IDE settings
echo   help               Show this help message
echo.
echo Example:
echo   build.bat build
echo   build.bat buildflash COM3
echo   build.bat monitor COM3
exit /b 1
