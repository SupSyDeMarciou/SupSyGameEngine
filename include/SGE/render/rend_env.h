#ifndef __SGE_RENDER_REND_ENV_H__
#define __SGE_RENDER_REND_ENV_H__

#include "../scene/scene.h"
#include "light.h"
#include "renderObject.h"

typedef shader func_backgroundRender(void);
typedef struct REBackground {
    void* data;
    func_free* freeData;
    func_backgroundRender* render;
} re_background;
typedef struct RenderEnvironment render_env;

#include "../builtin/backgrounds/simpleSky.h"

/// @brief Create a render environment
/// @return The newly created render environment
render_env* newRenderEnvironnment();
/// @brief Destroy a render environment
/// @param toDestroy The render environment to destroy
void freeRenderEnvironment(render_env* toDestroy);

/// @brief Set rendering camera for the render environment
/// @param camera The new rendering camera
void RESetRenderCamera(cam* camera);
/// @brief Get rendering camera for the render environment 
/// @return The current rendering camera
cam* REGetRenderCamera();

/// @brief Add a render object to a render environment
/// @param re The render environment
/// @param toAdd The render object to add
/// @return Wether a render object was added
bool REAddRenderObject(render_env* re, render_obj* toAdd);
/// @brief Remove a render object from a render environment
/// @param re The render environment
/// @param toAdd The render object to remove
/// @return Wether a render object was removed
bool RERemoveRenderObject(render_env* re, render_obj* toRemove);

/// @brief Add a light to a render environment
/// @param re The render environment
/// @param toAdd The light to add
/// @return Wether a light was added
bool REAddLight(render_env* re, light* toAdd);
/// @brief Remove a light from a render environment
/// @param re The render environment
/// @param toAdd The light to remove
/// @return Wether a light was removed
bool RERemoveLight(render_env* re, light* toRemove);
/// @brief Get index of light in lights array
/// @param re The render environment to search through
/// @param l The light to find the index of
/// @return The index of the light in the lights array OR -1 if not found in array
int REGetLightIndex(render_env* re, light* l);

/// @brief Set the current screen size
/// @param size New size
void RESetSize(uvec2 size);
/// @brief Get the current screen size
/// @return The current screen size
uvec2 REGetSize();
/// @brief Get the current screen HEIGHT to WIDTH ratio
/// @return The current screen ratio
float REGetScreenRatio();

/// @brief Set the ambiant color of this environment
/// @param newColor The new color to use
void RESetAmbiantColor(vec3 newColor);
/// @brief Set the background for the renders
/// @param bg The new background to use
void RESetBackground(re_background bg);
/// @brief Get the current background data
/// @return The current background data
void* REGetBackground();

/// @brief Get the scene output render buffer
/// @param re The render environment
/// @return The output render buffer
frame_buffer* REGetOutputFB(render_env* re);



void REUpdateGPUEnvironmentBuffer();
void REUpdateGPUCameraBuffer(cam* camera);
void REUpdateGPUObjectBuffer(render_obj* ro);
void REUpdateGPUObjectBuffer_Calculate(sc_obj* obj);

/// @brief Send some common data to the shader
/// @param s The recieving shader
/// @param re The render environment from which to extract the data
/// @note Updates "u_Time" and "u_Ratio"
void shaderSendCommonData(shader s);

/// @brief Render the screen quad
/// @param re The render environment
/// @param s The shader to use for rendering it
void RERenderQuad(render_env* re, shader s);

void RERenderScene_Start();
void RERenderBackgroundFB(frame_buffer* buffer);
void RECalculateRenderObjects();
void RERenderScene_Solid(frame_buffer* fb);
void RERenderScene_Transparent(frame_buffer* fb);

/// @brief Render every object in the scene to the specified frame buffer
/// @param buffer Frame buffer on which to render
void RERenderSceneFB(frame_buffer* buffer);
/// @brief Render every object in the scene
void RERenderScene();

/// @brief Apply Hight Dynamic Range to Low Dynamic Range conversion
/// @param source The HDR texture
/// @param dest The LDR output
void blitHdrToLdr(texture2D* source, frame_buffer* dest);
/// @brief Apply Hight Dynamic Range to Low Dynamic Range conversion
/// @param source The HDR frame buffer
/// @param dest The LDR output
void blitHdrToLdrFB(frame_buffer* source, frame_buffer* dest);
/// @brief Display the texture on screen
/// @param source The texture to display
void blitToScreen(texture2D* source);
/// @brief Display the frame buffer on screen
/// @param source The frame buffer to display
void blitToScreenFB(frame_buffer* source);

#endif