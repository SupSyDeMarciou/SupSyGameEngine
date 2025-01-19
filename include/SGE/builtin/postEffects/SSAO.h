#ifndef __SGE_BUILTIN_SSAO_H__
#define __SGE_BUILTIN_SSAO_H__

#include "../../../SGE.h"

/// @brief Initialize the necessary shaders for SSAO
/// @return If the logic has been initialized
bool initializeSSAO();
/// @brief Cleanly destroy all off the SSAO related memory
void terminateSSAOLogic();

/// @brief Set the maximum distance check for in the SSAO algorithm
/// @param distance The new distance
void SSAOSetDistance(float distance);
/// @brief Set the intensity of the effect
/// @param intensity The new intensity
void SSAOSetIntensity(float intensity);

/// @brief Applies SSAO to source
/// @param source The source buffer. Must contain depth buffer and normal buffer in addition to regular color
/// @param dest The destination buffer in which to store the result
void blitSSAOFB(frame_buffer* source, frame_buffer* dest);

#endif