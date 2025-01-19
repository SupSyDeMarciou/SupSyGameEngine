#ifndef __SGE_RENDER_SHADERS_POST_PROCESS_SHADER_H__
#define __SGE_RENDER_SHADERS_POST_PROCESS_SHADER_H__

#include "shader.h"
#include "../textures/frameBuffer.h"

/// @brief Post process shader object
typedef shader post_shader;

/// @brief Function type for sending post process data
typedef void func_postProcessRender(post_shader postProcessShader);

/// @brief Create a shader for post process effects
/// @param path Path to fragment shader
/// @return The compiled shader program
post_shader createPostProcessShader(const char* path);

/// @brief Move source to dest using a post process shader
/// @param s The post process shader
/// @param source The source 2D texture
/// @param dest The destination frame buffer
void blit(post_shader s, texture2D* source, frame_buffer* dest);
/// @brief Move source to dest using a post process shader
/// @param s The post process shader
/// @param source The source frame buffer
/// @param dest The destination frame buffer
void blitFB(post_shader s, frame_buffer* source, frame_buffer* dest);



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////// * * *  PRE-MADE POST PROCESS SHADERS  * * * /////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


extern post_shader noOpp; // Post process shader with no effect
void postShaderInitNoOpp();

#endif