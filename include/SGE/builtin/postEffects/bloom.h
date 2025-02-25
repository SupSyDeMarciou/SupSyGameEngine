#ifndef __SGE_BUILTIN_BLOOM_H__
#define __SGE_BUILTIN_BLOOM_H__

#include "../../../SGE.h"

/// @brief Initialize the necessary shaders for bloom
/// @return If the logic has been initialized
bool initializeBloom();
/// @brief Cleanly destroy all off the Bloom related memory
void terminateBloomLogic();

/// @brief Apply bloom to the input texture2D
/// @param source The input texture
/// @param dest The destination buffer in which to store the result
/// @param nbIter The number of passes
/// @param intensity The intensity of the composited bloom
void blitBloom(texture2D* source, frame_buffer* dest, uint nbIter, float intensity);
/// @brief Apply bloom to the input frameBuffer
/// @param source The input buffer (first texture in the frame buffer is used)
/// @param dest The destination buffer in which to store the result
/// @param nbIter The number of passes
/// @param intensity The intensity of the composited bloom
void blitBloomFB(frame_buffer* source, frame_buffer* dest, uint nbIter, float intensity);

#endif