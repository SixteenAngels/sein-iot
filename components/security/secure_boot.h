/**
 * @file secure_boot.h
 * @brief Secure Boot Implementation
 */

#ifndef SECURE_BOOT_H
#define SECURE_BOOT_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize secure boot system
 */
bool secure_boot_init(void);

/**
 * Verify firmware signature before execution
 */
bool secure_boot_verify_firmware(const uint8_t *firmware, uint32_t firmware_len,
                                 const uint8_t *signature);

/**
 * Verify FPGA bitstream signature
 */
bool secure_boot_verify_bitstream(const uint8_t *bitstream, uint32_t bitstream_len,
                                  const uint8_t *signature);

/**
 * Get device identity (public key)
 */
bool secure_boot_get_device_identity(uint8_t *public_key);

#ifdef __cplusplus
}
#endif

#endif /* SECURE_BOOT_H */
