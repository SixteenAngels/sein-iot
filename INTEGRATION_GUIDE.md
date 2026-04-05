# Integration & Deployment Guide

## Overview

This guide explains how to take the SEIN-IoT firmware framework and integrate it into a real-world deployment.

## Step 1: Choose Your Application Domain

SEIN-IoT is designed for these primary domains:

### Industrial IoT - Predictive Maintenance
- **Use Case**: Vibration monitoring, temperature anomaly detection, motor diagnostics
- **Sensors**: Accelerometers (ADXL345), thermocouples, current sensors
- **Models**: Fault classification CNNs
- **Example**: See [example_vibration_monitoring.c](example_vibration_monitoring.c)

### Healthcare - Wearable Monitoring  
- **Use Case**: ECG, SpO2, respiration rate, motion detection
- **Sensors**: Biomedical integrators, PPG sensors
- **Models**: Arrhythmia detection, activity classification
- **Privacy**: All processing on-device via HIPAA compliance

### Smart Agriculture - Crop Monitoring
- **Use Case**: Soil moisture, temperature, pest detection via imagery
- **Sensors**: Soil sensors, thermal cameras, spectral sensors
- **Models**: Disease detection, stress prediction
- **Battery**: 30-50 day lifecycle with MPPT charging

### Smart Grid - Energy Management
- **Use Case**: Power quality monitoring, fault detection, demand response
- **Sensors**: Voltage/current transducers, frequency monitoring
- **Models**: Anomaly detection, grid state classification
- **Security**: TLS 1.3 mandatory for critical infrastructure

## Step 2: Hardware Setup

### Bill of Materials (from SEIN-IoT design)

**Core Components**:
- ESP32-S3-WROOM-1: $3.50
- Lattice iCE40UP5K: $6.00
- ATECC608A: $0.65
- Power management ICs: $2.15
- Passives & connectors: $3.00
- **Total BOM**: ~$21.30/unit (volume 100)

**Sensors** (application-specific):
- **Vibration**: ADXL345 ($2)
- **Temperature**: DS18B20 ($0.50)
- **Humidity**: DHT22 ($3)
- **Light**: BH1750 ($1)

**Per-Unit Cost at Scale**:
- 100 units: $25-35 depending on sensors
- 1000 units: $18-25
- 10000 units: $15-20

### Assembly Checklist

- [ ] PCB fabrication (4-layer recommended)
- [ ] Component sourcing
- [ ] SMT assembly
- [ ] Manual assembly (connectors, sensors)
- [ ] Programming (bootloader + firmware)
- [ ] Functional test (all interfaces)
- [ ] Security provisioning (eFuse burning)

## Step 3: Firmware Customization

### 1. Sensor Integration

**Add your sensor to HAL layer** (`components/hal/`):

```c
// Create sensor_hal.h
typedef struct {
    uint8_t i2c_addr;
    uint16_t sample_rate_hz;
    uint8_t resolution_bits;
} sensor_config_t;

bool sensor_init(const sensor_config_t *config);
bool sensor_read(uint16_t *value);
void sensor_deinit(void);
```

**Update ADC HAL** for analog sensors:

```c
// In components/hal/adc_hal.c
uint16_t adc_hal_read(uint8_t channel);  // Sample single channel
bool adc_hal_read_multiple(uint8_t channel, uint16_t *samples, uint16_t count);
```

### 2. TinyML Model Integration

**Prepare your model**:

```bash
# 1. Train in TensorFlow
python train_model.py --output model.pb

# 2. Convert to TFLite
tf lite_converter --input_format=tf_saved_model \
                  --output_format=tflite \
                  --target_spec_compat_version=3 \
                  model.pb > model.tflite

# 3. Quantize to INT8
tf lite_converter --input_format=tf_saved_model \
                  --post_training_quantize \
                  model.pb > model_quantized.tflite

# 4. Convert to C header
xxd -i model_quantized.tflite > models/vibration_model.h
```

**Embed in firmware**:

```c
// In src/main.c
#include "vibration_model.h"

// In app_init():
inference_load_model(vibration_model_tflite, vibration_model_tflite_len);
```

###3. Task Customization

**Adjust FreeRTOS task parameters** in `include/config.h`:

```c
/* For compute-intensive applications */
#define STACK_SIZE_INFERENCE_TASK   8192  /* Increase heap for ML */
#define PRIORITY_FPGA_ACCEL_TASK    7     /* Raise for determinism */

/* For WiFi-heavy applications */
#define STACK_SIZE_WIFI_TASK        12288
#define STACK_SIZE_MQTT_TASK        6144

/* For low-power applications */
#define FREERTOS_TICK_RATE_HZ       100   /* Reduce tick rate */
#define CONFIG_ESP32S3_POWER_SAVE_DEFAULT 1
```

### 4. Network Connectivity

**Enable WiFi + MQTT**:

```c
// In src/freertos_tasks.c - wifi_task()
void wifi_task(void *pvParameters)
{
    /* Initialize WiFi */
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    
    /* Configure */
    wifi_config_t config = {
        .sta = {
            .ssid = "YourNetwork",
            .password = "YourPassword",
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &config));
    ESP_ERROR_CHECK(esp_wifi_start());
    
    /* Connect */
    ESP_ERROR_CHECK(esp_wifi_connect());
    
    /* MQTT publish loop */
    for (;;) {
        /* Read measurements */
        uint16_t sensor_value = adc_hal_read(0);
        
        /* Publish to broker */
        char topic[64];
        sprintf(topic, "devices/%s/sensor/temperature", DEVICE_ID);
        mqtt_publish(topic, (uint8_t *)&sensor_value, sizeof(sensor_value));
        
        vTaskDelay(pdMS_TO_TICKS(10000));  /* Every 10 seconds */
    }
}
```

## Step 4: Security Provisioning

### Secure Boot Setup

**1. Generate Keys**:
```bash
python -m esptool create_key --key_type=ecdsa256 secure_boot_key.pem
```

**2. Burn eFuse (one-time, irreversible)**:
```bash
esptool.py -p COM3 --before=default_reset --after=no_reset read_mac
esptool.py -p COM3 burn_key secure_boot SECURE_BOOT secure_boot_key.pem
```

**3. Enable in sdkconfig**:
```
CONFIG_SECURE_BOOT_ENABLED=y
CONFIG_SECURE_BOOT_V2_ENABLED=y
CONFIG_SECURE_BOOT_KEY_FILE="secure_boot_key.pem"
```

### ATECC608A Provisioning

**Burn device certificate**:

```c
// In secure_boot_init():
uint8_t public_key[64];
atecc608a_get_public_key(0, public_key);

/* Send public_key to provisioning server */
/* Server generates X.509 certificate */
/* Write certificate to flash and load into ATECC608A */
```

### OTA Update Security

**Create signed firmware**:

```bash
# Sign firmware binary
esptool.py sign_data --keyfile app_signing_key.pem \
                     build/sein_iot_firmware.bin \
                     > sein_iot_ota.bin

# Verify signature
openssl dgst -sha256 -verify public_key.pem \
             -signature signature.bin sein_iot_firmware.bin
```

## Step 5: Testing & Validation

### Unit Testing

Create `test/test_components.c`:

```c
bool test_crc16(void) {
    uint8_t data[] = {0x01, 0x02, 0x03};
    uint16_t expected_crc = 0x1234;  // Pre-computed
    uint16_t actual_crc = crc16_compute(data, 3);
    return (actual_crc == expected_crc);
}

bool test_fpga_communication(void) {
    if (!fpga_manager_init()) return false;
    if (!fpga_is_ready()) return false;
    return fpga_protocol_reset_fpga();
}

bool test_inference(void) {
    q15_t input[128];
    q15_t output[4];
    
    /* Load model, run inference */
    inference_load_model(model_data, model_size);
    return inference_run(input, 128, output, &output_len);
}
```

### Performance Benchmarking

```bash
# Build with debug symbols
idf.py build -DCMAKE_BUILD_TYPE=Debug

# Monitor timing
idf.py monitor -p COM3

# In your task, log timestamps:
uint32_t start = esp_timer_get_time();
// ... do work ...
uint32_t elapsed_us = esp_timer_get_time() - start;
ESP_LOGI(TAG, "Operation took %lu µs", elapsed_us);
```

### Power Profiling

**Measure current consumption**:

```bash
# Connect multimeter to 3.3V rail
# In your task:
vTaskDelay(pdMS_TO_TICKS(1000));  // Let current settle

/* Expected readings:
   - Idle: 15 mW
   - Processing: 85 mW
   - WiFi active: 150-180 mW
*/
```

## Step 6: Deployment

### Field Installation

**Pre-deployment checklist**:

- [ ] Firmware verified and signed
- [ ] Keys securely stored (ATECC608A)
- [ ] Certificate provisioned
- [ ] WiFi credentials configured (or via BLE pairing)
- [ ] Sensor calibration complete
- [ ] OTA endpoint configured
- [ ] Fallback firmware ready
- [ ] Monitoring/alerting enabled

### Monitoring & Analytics

**Recommended infrastructure**:

1. **Data Ingestion**: MQTT broker (Eclipse Mosquitto, AWS IoT, Azure IoT Hub)
2. **Time Series DB**: InfluxDB, TimescaleDB
3. **Analytics**: Grafana, Kibana
4. **Alerting**: PagerDuty, Twilio, custom webhooks

**Typical message format**:

```json
{
  "device_id": "SEIN-001",  
  "timestamp": 1698765432,
  "sensor": {
    "temperature": 42.5,
    "humidity": 65.2,
    "vibration_x": 125,
    "vibration_y": -45,
    "vibration_z": 230
  },
  "inference": {
    "class": "normal",
    "confidence": 0.954,
    "latency_ms": 18.2
  },
  "device": {
    "battery": 87.5,
    "rssi": -65,
    "uptime_s": 3600,
    "fw_version": "1.0.0"
  }
}
```

### Firmware Updates

**Release workflow**:

```bash
# 1. Tag release
git tag -a v1.1.0 -m "Add WiFi support"

# 2. Build release firmware
idf.py build -DBUILD_VERSION="1.1.0"

# 3. Sign for OTA
esptool.py sign_data --keyfile app_signing_key.pem \
                     build/sein_iot_firmware.bin

# 4. Upload to server
scp build/sein_iot_firmware.signed server:/firmware/

# 5. Notify devices via MQTT
mosquitto_pub -t "devices/+/command/ota" \
              -m '{"version":"1.1.0","url":"https://..."}'

# 6. Monitor rollout
# Track success rate, rollback if needed
```

## Troubleshooting Guide

### Common Issues & Solutions

**Issue**: FPGA not responding
```c
// Debug: Check SPI connectivity
bool test_spi(void) {
    uint16_t resp_len;
    return (spi_protocol_transaction(
        SPI_CMD_PING, NULL, 0, NULL, 0, &resp_len
    ) == SPI_STATUS_ACK);
}
```

**Issue**: Inference accuracy drops in field
```c
// Solution: Retrain on real data
// 1. Log raw sensor data for user
// 2. User sends data to cloud
// 3. Retrain model
// 4. Push updated model via OTA

// In firmware: log raw features periodically
if (sample_count % 1000 == 0) {
    mqtt_publish("devices/data/raw_features", 
                 (uint8_t *)features, feature_size);
}
```

**Issue**: WiFi causes timing issues
```c
// Solution: Prioritize FPGA/inference tasks
#define PRIORITY_FPGA_ACCEL_TASK    7  // Highest
#define PRIORITY_INFERENCE_TASK     6
#define PRIORITY_WIFI_TASK          3  // Lower priority

// Or use task suspension during critical sections
vTaskSuspend(wifi_task_handle);
// ... critical FPGA/inference work ...
vTaskResume(wifi_task_handle);
```

## Cost Analysis

| Component | Unit Cost | Quantity | Total |
|-----------|-----------|----------|-------|
| ESP32-S3 | $3.50 | 1 | $3.50 |
| FPGA | $6.00 | 1 | $6.00 |
| Secure Element | $0.65 | 1 | $0.65 |
| Power circuitry | $2.15 | 1 | $2.15 |
| Sensors (example) | $3.00 | 1 | $3.00 |
| PCB + Assembly | $5.00 | 1 | $5.00 |
| **Total (100-unit volume)** | | | **$20.30** |
| **Retail Price Target** | | | **$79-150** |
| **Gross Margin** | | | **75-85%** |

## Support & Resources

- **Hardware**: Contact Espressif, Lattice for technical support
- **Firmware**: Refer to documentation, ESP-IDF docs
- **Sensors**: Datasheets and application notes from manufacturers
- **Community**: GitHub issues, forums, Stack Overflow

## Next Steps

1. **Prototype**: Build one unit, validate design
2. **Pilot**: Deploy 5-10 units to real environment
3. **Iterate**: Collect data, retrain models, optimize
4. **Scale**: Move to manufacturing
5. **Monitor**: Continuous deployment of updates

---

**Remember**: Start simple, iterate based on real data, scale what works!
