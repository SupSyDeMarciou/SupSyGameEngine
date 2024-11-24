#ifndef __SGE_BUILTIN_SIMPLE_SKY_H__
#define __SGE_BUILTIN_SIMPLE_SKY_H__

#include "../../../SGE.h"


typedef struct SimpleSky {
    vec3 skyColorDay, skyColorNoon, skyColorNight;
    vec3 sunColorDay, sunColorNoon;
} simple_sky;

/// @brief Create a new simple sky background
/// @param skyColorDay The color of the sky during the day
/// @param skyColorNoon The color of the sky at noon
/// @param skyColorNight The color of the sky at night
/// @param sunColorDay The color of the sun during the day
/// @param sunColorNoon The color of the sun at noon
/// @param sun The sun used
/// @return The newly created simple sky background
simple_sky* newSimpleSky(vec3 skyColorDay, vec3 skyColorNoon, vec3 skyColorNight, vec3 sunColorDay, vec3 sunColorNoon, light* sun);
/// @brief Free a simple sky backround
/// @param toFree The simple sky backround to free
void freeSimpleSky(simple_sky* toFree);
/// @brief Package a simple sky into an REBackground
/// @param data The simple sky
/// @return The packaged REBackground
re_background simpleSkyAsREBackground(simple_sky* data);

/// @brief Get the sun referenced by a simple sky background
/// @param sk The simple sky background
/// @return The associated sun or NULL if there was none
light* simpleSkyGetSun(simple_sky* sk);
/// @brief Set the sun referenced by a simple sky background
/// @param sk The simple sky background
/// @param l The sun light to associate
/// @return Wether the light was successfuly added
/// @note The light must be of type "LIGHT_TYPE_DIRECTIONAL"
bool simpleSkySetSun(simple_sky* sk, light* l);
/// @brief Background render function for simple_sky 
/// @return The shader to use for the render
shader simpleSkyRender();

#endif