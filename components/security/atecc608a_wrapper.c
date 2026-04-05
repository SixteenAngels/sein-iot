/**
 * @file atecc608a_wrapper.c
 * @brief ATECC608A Implementation
 */

#include "atecc608a_wrapper.h"
#include "i2c_hal.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

static const char *TAG = "ATECC608A";
static bool atecc608a_initialized = false;

bool atecc608a_init(void)
{
    if (atecc608a_initialized) {
        return true;
    }

    /* Initialize I2C */
    if (!i2c_hal_init(1000000)) {
        ESP_LOGE(TAG, "I2C initialization failed");
        return false;
    }

    /* Verify ATECC608A is responding */
    uint8_t status;
    bool ret = i2c_hal_read(0x60, &status, 1);
    if (!ret) {
        ESP_LOGE(TAG, "ATECC608A not responding");
        return false;
    }

    atecc608a_initialized = true;
    ESP_LOGI(TAG, "ATECC608A initialized");
    return true;
}

void atecc608a_deinit(void)
{
    if (atecc608a_initialized) {
        i2c_hal_deinit();
        atecc608a_initialized = false;
    }
}

bool atecc608a_sha256(const uint8_t *data, size_t data_len,
                      uint8_t *digest)
{
    if (!atecc608a_initialized || !data || !digest) {
        return false;
    }

    /* Build SHA256 command for ATECC608A */
    uint8_t cmd[] = {0x47, 0x01, 0x00, 0x00};  /* SHA256 command */

    if (!i2c_hal_write(0x60, cmd, sizeof(cmd))) {
        return false;
    }

    if (!i2c_hal_write(0x60, data, data_len)) {
        return false;
    }

    vTaskDelay(pdMS_TO_TICKS(10));

    if (!i2c_hal_read(0x60, digest, ATECC608A_SHA256_DIGEST_SIZE)) {
        return false;
    }

    return true;
}

bool atecc608a_sign(uint8_t key_slot,
                    const uint8_t *data, size_t data_len,
                    uint8_t *signature)
{
    if (!atecc608a_initialized || !data || !signature) {
        return false;
    }

    if (data_len != ATECC608A_SHA256_DIGEST_SIZE) {
        ESP_LOGE(TAG, "ECDSA requires 32-byte SHA256 digest");
        return false;
    }

    /* Build ECDSA command */
    uint8_t cmd[] = {0x41, 0x00, key_slot, 0x00};

    if (!i2c_hal_write(0x60, cmd, sizeof(cmd))) {
        return false;
    }

    if (!i2c_hal_write(0x60, data, data_len)) {
        return false;
    }

    vTaskDelay(pdMS_TO_TICKS(80));  /* ECDSA takes ~60ms */

    if (!i2c_hal_read(0x60, signature, ATECC608A_SIGNATURE_SIZE)) {
        return false;
    }

    return true;
}

bool atecc608a_verify(const uint8_t *public_key,
                      const uint8_t *data, size_t data_len,
                      const uint8_t *signature)
{
    if (!atecc608a_initialized || !public_key || !data || !signature) {
        return false;
    }

    /* Build VERIFY command */
    uint8_t cmd[] = {0x45, 0x00, 0x00, 0x00};

    if (!i2c_hal_write(0x60, cmd, sizeof(cmd))) {
        return false;
    }

    if (!i2c_hal_write(0x60, public_key, ATECC608A_PUBLIC_KEY_SIZE)) {
        return false;
    }

    vTaskDelay(pdMS_TO_TICKS(100));

    uint8_t result = 0;
    if (!i2c_hal_read(0x60, &result, 1)) {
        return false;
    }

    return (result == 0x00);  /* 0x00 = valid signature */
}

bool atecc608a_get_public_key(uint8_t key_slot, uint8_t *public_key)
{
    if (!atecc608a_initialized || !public_key) {
        return false;
    }

    /* Build GENKEY command */
    uint8_t cmd[] = {0x40, 0x04, key_slot, 0x00};

    if (!i2c_hal_write(0x60, cmd, sizeof(cmd))) {
        return false;
    }

    vTaskDelay(pdMS_TO_TICKS(150));

    if (!i2c_hal_read(0x60, public_key, ATECC608A_PUBLIC_KEY_SIZE)) {
        return false;
    }

    return true;
}

bool atecc608a_random(uint8_t *random_data, size_t len)
{
    if (!atecc608a_initialized || !random_data) {
        return false;
    }

    if (len > ATECC608A_RANDOM_SIZE) {
        len = ATECC608A_RANDOM_SIZE;
    }

    uint8_t cmd[] = {0x1B, 0x00, 0x00, 0x00};

    if (!i2c_hal_write(0x60, cmd, sizeof(cmd))) {
        return false;
    }

    vTaskDelay(pdMS_TO_TICKS(10));

    if (!i2c_hal_read(0x60, random_data, len)) {
        return false;
    }

    return true;
}

bool atecc608a_read_counter(uint8_t counter_id, uint32_t *counter_value)
{
    if (!atecc608a_initialized || !counter_value) {
        return false;
    }

    /* Build COUNTER command */
    uint8_t cmd[] = {0x24, 0x00, counter_id, 0x00};
    uint8_t counter_bytes[4];

    if (!i2c_hal_write(0x60, cmd, sizeof(cmd))) {
        return false;
    }

    vTaskDelay(pdMS_TO_TICKS(5));

    if (!i2c_hal_read(0x60, counter_bytes, sizeof(counter_bytes))) {
        return false;
    }

    *counter_value = (counter_bytes[0] << 24) | (counter_bytes[1] << 16) |
                     (counter_bytes[2] << 8) | counter_bytes[3];

    return true;
}

bool atecc608a_increment_counter(uint8_t counter_id, uint32_t *counter_value)
{
    if (!atecc608a_initialized || !counter_value) {
        return false;
    }

    /* Build COUNTER command with increment */
    uint8_t cmd[] = {0x24, 0x01, counter_id, 0x00};

    if (!i2c_hal_write(0x60, cmd, sizeof(cmd))) {
        return false;
    }

    vTaskDelay(pdMS_TO_TICKS(5));

    uint8_t counter_bytes[4];
    if (!i2c_hal_read(0x60, counter_bytes, sizeof(counter_bytes))) {
        return false;
    }

    *counter_value = (counter_bytes[0] << 24) | (counter_bytes[1] << 16) |
                     (counter_bytes[2] << 8) | counter_bytes[3];

    return true;
}

bool atecc608a_aes_encrypt(uint8_t key_slot,
                          const uint8_t *plaintext,
                          uint8_t *ciphertext)
{
    if (!atecc608a_initialized || !plaintext || !ciphertext) {
        return false;
    }

    /* Build AES ENC command */
    uint8_t cmd[] = {0x37, 0x00, 0x00, key_slot};

    if (!i2c_hal_write(0x60, cmd, sizeof(cmd))) {
        return false;
    }

    if (!i2c_hal_write(0x60, plaintext, 16)) {  /* AES block: 16 bytes */
        return false;
    }

    vTaskDelay(pdMS_TO_TICKS(10));

    if (!i2c_hal_read(0x60, ciphertext, 16)) {
        return false;
    }

    return true;
}

bool atecc608a_aes_decrypt(uint8_t key_slot,
                          const uint8_t *ciphertext,
                          uint8_t *plaintext)
{
    if (!atecc608a_initialized || !ciphertext || !plaintext) {
        return false;
    }

    /* Build AES DEC command */
    uint8_t cmd[] = {0x37, 0x80, 0x00, key_slot};

    if (!i2c_hal_write(0x60, cmd, sizeof(cmd))) {
        return false;
    }

    if (!i2c_hal_write(0x60, ciphertext, 16)) {
        return false;
    }

    vTaskDelay(pdMS_TO_TICKS(10));

    if (!i2c_hal_read(0x60, plaintext, 16)) {
        return false;
    }

    return true;
}
