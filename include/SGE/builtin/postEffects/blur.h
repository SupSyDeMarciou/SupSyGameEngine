#ifndef __SGE_BUILTIN_BLUR
#define __SGE_BUILTIN_BLUR

#include "../../../SGE.h"

#define SGE_BUILTIN_BLUR_MAX_ITER 8 // Maximum number of bluring steps that can be taken

/// @brief Initialize the necessary shaders for blur
/// @return If the logic has been initialized
bool initializeBlurLogic();

/// @brief Apply blur to the input texture2D
/// @param source The texture to blur
/// @param init The shader which will transition the texture into the hidden frame buffers (must be configured before call, set to 'noOpp' if no transition needed)
/// @param iter The number of steps to take while blurring
/// @return A pointer to the blured result
/// @note The destination is in a fixed resolution and the pointer must not be tinkered with. It is readonly
const texture2D* applyBlur(texture2D* source, post_shader init, uint iter);

#endif