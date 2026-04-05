/**
 * @file inference_engine.h
 * @brief TinyML Inference Engine
 */

#ifndef INFERENCE_ENGINE_H
#define INFERENCE_ENGINE_H

#include <stdint.h>
#include <stdbool.h>
#include "sein_iot.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Model Configuration */
#define TINYML_MAX_INPUTS       128
#define TINYML_MAX_OUTPUTS      10
#define TINYML_TENSOR_ARENA_SIZE (256 * 1024)

/**
 * Initialize TinyML inference engine
 */
bool inference_engine_init(void);

/**
 * Load TFLite model into memory
 */
bool inference_load_model(const uint8_t *model_data, uint32_t model_size);

/**
 * Run inference on input tensor
 */
bool inference_run(const q15_t *input_data, uint16_t input_len,
                   q15_t *output_data, uint16_t *output_len);

/**
 * Get model input size
 */
uint16_t inference_get_input_size(void);

/**
 * Get model output size
 */
uint16_t inference_get_output_size(void);

/**
 * Deinitialize inference engine
 */
void inference_engine_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* INFERENCE_ENGINE_H */
