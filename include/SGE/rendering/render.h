#ifndef __SGE_RENDER_RENDER_H__
#define __SGE_RENDER_RENDER_H__

#include "../SGEstructures.h"

#include "../scene/scene.h"
#include "light.h"
#include "renderObject.h"


typedef struct SkyVariables sky_vars;
void REbackground_skySetColors(vec3 sunColorDay, vec3 sunColorNoon, vec3 skyColorDay, vec3 skyColorNoon, vec3 skyColorNight);
bool REbackground_skySetSun(light* l);
shader REbackground_skyRender();


typedef struct RenderEnvironment render_env;

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

/// @brief Send the environment data (camera included) to the shader
/// @param s The recieving shader
/// @param re The render environment from which to extract the data
void shaderSendEnvironmentData(shader s);

/// @brief Set the current screen size
/// @param size New size
void RESetCurrentSize(uvec2 size);
/// @brief Get the current screen size
/// @return The current screen size
uvec2 REGetCurrentSize();
/// @brief Get the current screen HEIGHT to WIDTH ratio
/// @return The current screen ratio
float REGetScreenRatio();

/// @brief Render the screen quad
/// @param re The render environment
/// @param s The shader to use for rendering it
void RERenderQuad(render_env* re, shader s);

/// @brief Get the scene output render buffer
/// @param re The render environment
/// @return The output render buffer
frame_buffer* REGetOutputFB(render_env* re);

/// @brief Set the ambiant color of this environment
/// @param newColor The new color to use
void RESetAmbiantColor(vec4 newColor);
/// @brief Set the background for the renders
/// @param backgroundRenderFunc The function which sends all the necessary data before rendering
/// @param backgroundData Additionnal data
/// @return The last background data which was stored
void* RESetBackground(shader (*backgroundRenderFunc)(void*), void* backgroundData);
/// @brief Get the current background data
/// @return The current background data
void* REGetBackgroundData();

void REUpdateGPUEnvironmentData();
void REUpdateGPUCameraData(cam* camera);
void REUpdateGPURenderObjectData(render_obj* ro);
void REUpdateGPURenderObjectData_Calculate(sc_obj* obj);
/// @brief Render every object in the scene
void RERenderScene();
/// @brief Render every object in the scene to the specified frame buffer
/// @param buffer Frame buffer on which to render
void RERenderSceneFB(frame_buffer* buffer);

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