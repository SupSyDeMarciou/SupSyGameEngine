#ifndef __SGE_RENDER_RENDER_H__
#define __SGE_RENDER_RENDER_H__

#include "../constants.h"

/// @brief The different culling modes
typedef enum Render_FaceCulling {
    RENDER_CULL_NONE = 0b00000000,
    RENDER_CULL_BACK = 0b00000001,
    RENDER_CULL_FRONT = 0b00000010,
    RENDER_CULL_BOTH = 0b00000011
} render_cull_face;

/// @brief The different culling modes
typedef enum Render_BlendFunction {
    RENDER_BLEND_FUNC_ZERO = GL_ZERO,
    RENDER_BLEND_FUNC_ONE = GL_ONE,

    RENDER_BLEND_FUNC_SRC_COLOR = GL_SRC_COLOR,
    RENDER_BLEND_FUNC_SRC_NEG_COLOR = GL_ONE_MINUS_SRC_COLOR,
    RENDER_BLEND_FUNC_SRC_ALPHA = GL_SRC_ALPHA,
    RENDER_BLEND_FUNC_SRC_NEG_ALPHA = GL_ONE_MINUS_SRC_ALPHA,

    RENDER_BLEND_FUNC_CONST_COLOR = GL_CONSTANT_COLOR,
    RENDER_BLEND_FUNC_NEG_CONST_COLOR = GL_ONE_MINUS_CONSTANT_COLOR,
    RENDER_BLEND_FUNC_CONST_ALPHA = GL_CONSTANT_ALPHA,
    RENDER_BLEND_FUNC_NEG_CONST_ALPHA = GL_ONE_MINUS_CONSTANT_ALPHA,

    RENDER_BLEND_FUNC_DST_COLOR = GL_DST_COLOR,
    RENDER_BLEND_FUNC_DST_NEG_COLOR = GL_ONE_MINUS_DST_COLOR,
    RENDER_BLEND_FUNC_DST_ALPHA = GL_DST_ALPHA,
    RENDER_BLEND_FUNC_DST_NEG_ALPHA = GL_ONE_MINUS_DST_ALPHA,
} render_blend_func;

typedef enum Render_DepthFunction {
    RENDER_DEPTH_FUNC_NEVER = GL_NEVER,
    RENDER_DEPTH_FUNC_LESS_THAN = GL_LESS,
    RENDER_DEPTH_FUNC_LESS_EQUAL = GL_LEQUAL,
    RENDER_DEPTH_FUNC_GREATER_EQUAL = GL_GEQUAL,
    RENDER_DEPTH_FUNC_GREATER_THAN = GL_GREATER,
    RENDER_DEPTH_FUNC_ALWAYS = GL_ALWAYS
} render_depth_func;

/// @brief Get information on the user hardware
/// @param value The "GL_..." information to get
/// @return The value (if tracked by the SGE)
uint renderGetOpenGLIntergerv(GLint value);

/// @brief Set the current face cull mode
/// @param cullFace The face being discarded
void renderSetCullFace(render_cull_face cullFace);
/// @brief Get the current face cull mode
/// @return The current cull face mode
render_cull_face renderGetCullFace();
/// @brief Set wether depth testing is enabled
/// @param doTest Wether depth testing is enabled
void renderSetDepthTest(bool doTest);
/// @brief Get wether depth testing is enabled
/// @return Wether depth testing is enabled
bool renderGetDepthTest();
/// @brief Set wether depth writting is enabled
/// @param doWrite Wether depth writting is enabled
void renderSetDepthWrite(bool doWrite);
/// @brief Get wether depth writting is enabled
/// @return Wether depth writting is enabled
bool renderGetDepthWrite();
/// @brief Set the function used when testing depth
/// @param func The function to use
void renderSetDepthFunc(render_depth_func func);
/// @brief Get the function used when testing depth
/// @return The function used
bool renderGetDepthFunc();
/// @brief Set wether stencil testing is enabled
/// @param doTest Wether stencil testing is enabled
void renderSetStencilTest(bool doTest);
/// @brief Get wether stencil testing is enabled
/// @return Wether stencil testing is enabled
bool renderGetStencilTest();
/// @brief Set wether blending is enabled
/// @param doBlend Wether blending is enabled
void renderSetBlend(bool doBlend);
/// @brief Get wether blending is enabled
/// @return Wether blending is enabled
bool renderGetBlend();
/// @brief Set the function to use when blending
/// @param source The blending mode for the "top" fragment
/// @param dest The blending mode for the "bottom" fragment
void renderSetBlendFunc(render_blend_func source, render_blend_func dest);
/// @brief Get the function to use when blending
/// @return The two functions used when blending
uvec2 renderGetBlendFunc();

bool renderInitialize();

#endif