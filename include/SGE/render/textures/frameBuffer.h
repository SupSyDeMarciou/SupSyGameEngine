#ifndef __SGE_RENDER_TEXTURES_FRAME_BUFFER_H__
#define __SGE_RENDER_TEXTURES_FRAME_BUFFER_H__

#include "texture.h"

typedef enum DepthStencilFormat {
    FB_DS_D16 = GL_DEPTH_COMPONENT16,
    FB_DS_D24 = GL_DEPTH_COMPONENT24,
    FB_DS_D32 = GL_DEPTH_COMPONENT32,
    FB_DS_D32F = GL_DEPTH_COMPONENT32F,
    FB_DS_S1 = GL_STENCIL_INDEX1,
    FB_DS_S4 = GL_STENCIL_INDEX4,
    FB_DS_S8 = GL_STENCIL_INDEX8,
    FB_DS_S16 = GL_STENCIL_INDEX16,
    FB_DS_D24S8 = GL_DEPTH24_STENCIL8,
    FB_DS_D32FS8 = GL_DEPTH32F_STENCIL8,
} depth_stenc_form;

typedef struct FrameBuffer frame_buffer;

/// @brief Create a frame buffer ON STACK
/// @param size The size
/// @return The newly created frame buffer
frame_buffer createFrameBuffer(uvec2 size);
/// @brief Destroy a frame buffer
/// @param toDestroy The frame buffer to destroy
void destroyFrameBuffer(frame_buffer toDestroy);
/// @brief Create a new frame buffer
/// @param size The size
/// @return The newly created frame buffer
frame_buffer* newFrameBuffer(uvec2 size);
/// @brief Free a frame buffer
/// @param toFree The frame buffer to free
void freeFrameBuffer(frame_buffer* toFree);

/// @brief Check if a frame buffer is complete (according to OpenGL specification)
/// @param buffer The buffer to check
/// @return Wether the buffer is complete
bool isFBComplete(frame_buffer* buffer);
/// @brief Crash application if frame buffer is not complete
/// @param buffer The buffer to check
#define FBFailIfNotComplete(buffer) do { if (!isFBComplete(buffer)) failWithError("Frame buffer not complete"); } while (0)
/// @brief Get the size of a frame buffer
/// @param buffer The frame buffer
/// @return The size of the frame buffer
uvec2 FBGetSize(frame_buffer* buffer);

///////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// COLOR //////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

/// @brief Attach a color texture to a frame buffer
/// @param buffer The buffer
/// @param format The format of the texture to add
/// @note Only there is an upper bound on the number of textures you can attach to a frame buffer
void FBAttachColor(frame_buffer* buffer, tex_form format);

///////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// DEPTH-STENCIL //////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

/// @brief Attach a depth, stencil or depth-stencil texture to the frame buffer
/// @param buffer The buffer
/// @param format The format of the texture to add
/// @note Only one depth-stencil may be attached to a specific frame buffer
void FBAttachDepthStencil(frame_buffer* buffer, depth_stenc_form format);

///////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// SHADER UTILS //////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

/// @brief Bind a frame buffer
/// @param buffer The buffer 
/// @param clearBits The textures to clear (color, depth and/or stencil)
/// @param clearColor The clearing color
void FBBind(frame_buffer* buffer, uint clearBits, vec4 clearColor);
/// @brief Bind the window frame buffer
/// @param width The target width to render to
/// @param height The target height to render to
/// @param clearBits The textures to clear (color, depth and/or stencil)
/// @param clearColor The clearing color
void FBUnbind(uint width, uint height, uint clearBits, vec4 clearColor);
/// @brief Get the OpenGL ID of a frame buffer
/// @param buffer The buffer
/// @return The requested ID
/// @note This is the object identifier OpenGL returns when creating objects on the GPU
GLuint FBGetGlID(frame_buffer* buffer);
/// @brief Set a shader 2D texture from a frame buffer color texture
/// @param s Shader ID
/// @param buffer The desired buffer
/// @param fieldName Field name of the variable to set
/// @param textureID The texture's position in shader (relative to first texture used)
/// @param texturePosition The texture's position in frame buffer (order in which it was added)
void shaderSetFBColor(shader s, frame_buffer* buffer, const char* fieldName, uint textureID, uint texturePosition);
/// @brief Get a color texture from a frame buffer
/// @param buffer The frame buffer
/// @param texNb The index of the color texture
/// @return The 2D texture
texture2D* FBGetColor(frame_buffer* buffer, uint texNb);
/// @brief Get the OpenGL ID of a 2D texture from a frame buffer color texture
/// @param buffer The frame buffer
/// @param texNb The index of the color texture
/// @return The requested ID
/// @note This is the object identifier OpenGL returns when creating objects on the GPU
GLuint FBGetColorGlID(frame_buffer* buffer, uint texNb);
/// @brief Set a shader 2D texture from a frame buffer depth-stencil texture
/// @param s Shader ID
/// @param buffer The desired buffer
/// @param fieldName Field name of the variable to set
/// @param textureID The texture's position in shader (relative to first texture used)
void shaderSetFBDepthStencil(shader s, frame_buffer* buffer, const char* fieldName, uint textureID);
/// @brief Get the OpenGL ID of a 2D texture from a frame buffer depth-stencil texture
/// @param buffer The frame buffer
/// @return The requested ID
/// @note This is the object identifier OpenGL returns when creating objects on the GPU
GLuint FBGetDepthStencilGlID(frame_buffer* buffer);

#endif