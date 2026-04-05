/**
 * @file atecc608a_wrapper.h
 * @brief ATECC608A Secure Element Wrapper
 */

#ifndef ATECC608A_WRAPPER_H
#define ATECC608A_WRAPPER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Key Slot Definitions */
#define ATECC608A_KEY_SLOT_DEVICE_IDENTITY  0
#define ATECC608A_KEY_SLOT_TLS_CERT         4
#define ATECC608A_KEY_SLOT_BITSTREAM_SIGN   8
#define ATECC608A_KEY_SLOT_APP_KEY_0        12
#define ATECC608A_KEY_SLOT_APP_KEY_1        13

/* Cryptographic Output Sizes */
#define ATECC608A_SHA256_DIGEST_SIZE        32  /* 256 bits */
#define ATECC608A_SIGNATURE_SIZE            64  /* P-256 ECDSA: 64 bytes */
#define ATECC608A_PUBLIC_KEY_SIZE           64  /* P-256 public key: 64 bytes */
#define ATECC608A_RANDOM_SIZE               32  /* 256-bit random */

/**
 * Initialize ATECC608A secure element
 */
bool atecc608a_init(void);

/**
 * Deinitialize ATECC608A
 */
void atecc608a_deinit(void);

/**
 * Compute SHA-256 hash
 */
bool atecc608a_sha256(const uint8_t *data, size_t data_len,
                      uint8_t *digest);

/**
 * Sign data with private key (ECDSA P-256)
 */
bool atecc608a_sign(uint8_t key_slot,
                    const uint8_t *data, size_t data_len,
                    uint8_t *signature);

/**
 * Verify signature with public key (ECDSA P-256)
 */
bool atecc608a_verify(const uint8_t *public_key,
                      const uint8_t *data, size_t data_len,
                      const uint8_t *signature);

/**
 * Get public key from key slot
 */
bool atecc608a_get_public_key(uint8_t key_slot, uint8_t *public_key);

/**
 * Generate random number
 */
bool atecc608a_random(uint8_t *random_data, size_t len);

/**
 * Read counter value
 */
bool atecc608a_read_counter(uint8_t counter_id, uint32_t *counter_value);

/**
 * Increment counter value
 */
bool atecc608a_increment_counter(uint8_t counter_id, uint32_t *counter_value);

/**
 * AES-128 encrypt
 */
bool atecc608a_aes_encrypt(uint8_t key_slot,
                          const uint8_t *plaintext,
                          uint8_t *ciphertext);

/**
 * AES-128 decrypt
 */
bool atecc608a_aes_decrypt(uint8_t key_slot,
                          const uint8_t *ciphertext,
                          uint8_t *plaintext);

#ifdef __cplusplus
}
#endif

#endif /* ATECC608A_WRAPPER_H */
