/**
 * @file fpga_dsp.c
 * @brief FPGA-Accelerated DSP Implementation
 */

#include "fpga_dsp.h"
#include "fpga_manager.h"
#include "esp_log.h"
#include <math.h>

static const char *TAG = "FPGA_DSP";

bool dsp_fir_filter(const q15_t *input, uint16_t input_len,
                    const q15_t *coefficients, uint16_t num_taps,
                    q15_t *output, uint16_t *output_len)
{
    if (!input || !coefficients || !output || !output_len) {
        return false;
    }

    /* Configure FIR on FPGA */
    if (!fpga_configure_fir(num_taps, coefficients)) {
        return false;
    }

    /* Execute FIR */
    if (!fpga_execute_fir(input, input_len, output, output_len)) {
        return false;
    }

    ESP_LOGI(TAG, "FIR filter: %d samples -> %d output samples", input_len, *output_len);
    return true;
}

bool dsp_fft(const q15_t *input, uint16_t num_points,
             complex_q15_t *spectrum, uint16_t *output_len)
{
    if (!input || !spectrum || !output_len) {
        return false;
    }

    /* Verify FFT size is power of 2 */
    if ((num_points & (num_points - 1)) != 0) {
        ESP_LOGE(TAG, "FFT size must be power of 2: %d", num_points);
        return false;
    }

    /* Configure FFT on FPGA */
    if (!fpga_configure_fft(num_points)) {
        return false;
    }

    /* Execute FFT */
    if (!fpga_execute_fft(input, num_points, spectrum, output_len)) {
        return false;
    }

    ESP_LOGI(TAG, "FFT: %d points, %d output bins", num_points, *output_len);
    return true;
}

bool dsp_ifft(const complex_q15_t *spectrum, uint16_t num_points,
              q15_t *output, uint16_t *output_len)
{
    /* IFFT would require separate FPGA implementation */
    /* For now, return placeholder */
    (void)spectrum;
    (void)num_points;
    (void)output;
    (void)output_len;
    return false;
}

void dsp_magnitude_spectrum(const complex_q15_t *spectrum, uint16_t len,
                            q15_t *magnitude)
{
    if (!spectrum || !magnitude) {
        return;
    }

    /* Convert Q15 complex to magnitude */
    for (uint16_t i = 0; i < len; i++) {
        int32_t real = spectrum[i].real;
        int32_t imag = spectrum[i].imag;
        int32_t mag_squared = real * real + imag * imag;
        magnitude[i] = (q15_t)((int16_t)((mag_squared >> 15) & 0xFFFF));
    }
}
