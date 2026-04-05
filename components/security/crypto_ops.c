/**
 * @file crypto_ops.c
 * @brief Cryptographic Operations Implementation
 */

#include "crypto_ops.h"
#include "atecc608a_wrapper.h"
#include "esp_log.h"

static const char *TAG = "CRYPTO_OPS";

bool crypto_verify_signature(const uint8_t *data, size_t data_len,
                            const uint8_t *signature)
{
    uint8_t hash[32];

    /* Compute SHA-256 of data */
    if (!atecc608a_sha256(data, data_len, hash)) {
        ESP_LOGE(TAG, "SHA256 computation failed");
        return false;
    }

    /* Get public key from device */
    uint8_t public_key[64];
    if (!atecc608a_get_public_key(0, public_key)) {
        ESP_LOGE(TAG, "Failed to get public key");
        return false;
    }

    /* Verify signature */
    return atecc608a_verify(public_key, hash, 32, signature);
}

bool crypto_create_signature(uint8_t key_slot,
                            const uint8_t *data, size_t data_len,
                            uint8_t *signature)
{
    uint8_t hash[32];

    /* Compute SHA-256 of data */
    if (!atecc608a_sha256(data, data_len, hash)) {
        ESP_LOGE(TAG, "SHA256 computation failed");
        return false;
    }

    /* Sign the hash */
    return atecc608a_sign(key_slot, hash, 32, signature);
}

bool crypto_sha256(const uint8_t *data, size_t data_len,
                   uint8_t *hash)
{
    return atecc608a_sha256(data, data_len, hash);
}

bool crypto_random_bytes(uint8_t *buffer, size_t len)
{
    return atecc608a_random(buffer, len);
}
