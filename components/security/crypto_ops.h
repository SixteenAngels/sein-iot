/**
 * @file crypto_ops.h
 * @brief High-level Cryptographic Operations
 */

#ifndef CRYPTO_OPS_H
#define CRYPTO_OPS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Verify data signature
 */
bool crypto_verify_signature(const uint8_t *data, size_t data_len,
                            const uint8_t *signature);

/**
 * Create data signature
 */
bool crypto_create_signature(uint8_t key_slot,
                            const uint8_t *data, size_t data_len,
                            uint8_t *signature);

/**
 * Compute SHA-256 hash
 */
bool crypto_sha256(const uint8_t *data, size_t data_len,
                   uint8_t *hash);

/**
 * Generate random bytes
 */
bool crypto_random_bytes(uint8_t *buffer, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* CRYPTO_OPS_H */
