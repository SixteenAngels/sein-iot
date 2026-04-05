/**
 * @file fpga_manager.h
 * @brief FPGA Management and Configuration
 */

#ifndef FPGA_MANAGER_H
#define FPGA_MANAGER_H

#include <stdint.h>
#include <stdbool.h>
#include "sein_iot.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize FPGA subsystem
 */
bool fpga_manager_init(void);

/**
 * Deinitialize FPGA
 */
void fpga_manager_deinit(void);

/**
 * Load bitstream from flash
 */
bool fpga_load_bitstream(const uint8_t *bitstream_data, uint32_t bitstream_len);

/**
 * Configure FPGA for FIR filtering operation
 */
bool fpga_configure_fir(uint16_t num_taps, const q15_t *coefficients);

/**
 * Configure FPGA for FFT operation
 */
bool fpga_configure_fft(uint16_t fft_size);

/**
 * Configure FPGA for MAC operations
 */
bool fpga_configure_mac(uint16_t num_inputs, const q15_t *weights);

/**
 * Execute FIR filter on FPGA
 */
bool fpga_execute_fir(const q15_t *input_samples, uint16_t num_samples,
                      q15_t *output_samples, uint16_t *output_len);

/**
 * Execute FFT on FPGA
 */
bool fpga_execute_fft(const q15_t *input_samples, uint16_t num_samples,
                      complex_q15_t *output_spectrum, uint16_t *output_len);

/**
 * Execute MAC operation on FPGA
 */
bool fpga_execute_mac(const q15_t *inputs, uint16_t num_inputs,
                      q15_t *output);

/**
 * Poll FPGA ready status
 */
bool fpga_is_ready(void);

/**
 * Get FPGA status
 */
fpga_status_t fpga_get_status(void);

#ifdef __cplusplus
}
#endif

#endif /* FPGA_MANAGER_H */
