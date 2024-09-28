#ifndef __SGE_BUILTIN_SSAO
#define __SGE_BUILTIN_SSAO

#include <SupSy/SGE.h>

/// @brief Initialize the necessary shaders for SSAO
/// @return If the logic has been initialized
bool initializeSSAOLogic();

/// @brief Applies SSAO to source
/// @param source The source buffer. Must contain depth buffer and normal buffer in addition to regular color
/// @param dest The destination buffer in which to store the result
void blitSSAOFB(frame_buffer* source, frame_buffer* dest);

#endif