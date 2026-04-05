/**
 * @file fpga_dsp.h
 * @brief FPGA-Accelerated DSP Algorithms
 */

#ifndef FPGA_DSP_H
#define FPGA_DSP_H

#include <stdint.h>
#include <stdbool.h>
#include "sein_iot.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Perform FIR filtering on FPGA
 */
bool dsp_fir_filter(const q15_t *input, uint16_t input_len,
                    const q15_t *coefficients, uint16_t num_taps,
                    q15_t *output, uint16_t *output_len);

/**
 * Perform FFT on FPGA
 */
bool dsp_fft(const q15_t *input, uint16_t num_points,
             complex_q15_t *spectrum, uint16_t *output_len);

/**
 * Perform IFFT on FPGA
 */
bool dsp_ifft(const complex_q15_t *spectrum, uint16_t num_points,
              q15_t *output, uint16_t *output_len);

/**
 * Compute magnitude spectrum from complex FFT output
 */
void dsp_magnitude_spectrum(const complex_q15_t *spectrum, uint16_t len,
                            q15_t *magnitude);

#ifdef __cplusplus
}
#endif

#endif /* FPGA_DSP_H */
