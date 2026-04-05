/**
 * @file secure_boot.c
 * @brief Secure Boot Implementation
 */

#include "secure_boot.h"
#include "crypto_ops.h"
#include "atecc608a_wrapper.h"
#include "esp_log.h"

static const char *TAG = "SECURE_BOOT";

bool secure_boot_init(void)
{
    if (!atecc608a_init()) {
        ESP_LOGE(TAG, "ATECC608A initialization failed");
        return false;
    }

    ESP_LOGI(TAG, "Secure boot initialized");
    return true;
}

bool secure_boot_verify_firmware(const uint8_t *firmware, uint32_t firmware_len,
                                 const uint8_t *signature)
{
    if (!firmware || firmware_len == 0 || !signature) {
        ESP_LOGE(TAG, "Invalid firmware parameters");
        return false;
    }

    return crypto_verify_signature(firmware, firmware_len, signature);
}

bool secure_boot_verify_bitstream(const uint8_t *bitstream, uint32_t bitstream_len,
                                  const uint8_t *signature)
{
    if (!bitstream || bitstream_len == 0 || !signature) {
        ESP_LOGE(TAG, "Invalid bitstream parameters");
        return false;
    }

    return crypto_verify_signature(bitstream, bitstream_len, signature);
}

bool secure_boot_get_device_identity(uint8_t *public_key)
{
    if (!public_key) {
        return false;
    }

    return atecc608a_get_public_key(0, public_key);
}
