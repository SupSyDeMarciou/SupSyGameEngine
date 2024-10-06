#ifndef __SGE_RENDERING_LIGHT
#define __SGE_RENDERING_LIGHT

#include "../SGEstructures.h"
#include "../scene/sc_object.h"

/// @brief Type of a light external data
enum LightType {
    LIGHT_TYPE_NONE,
    LIGHT_TYPE_DIRECTIONAL,
    LIGHT_TYPE_POINT,
    LIGHT_TYPE_SPOT,
    LIGHT_TYPE_AREA
};

/// @brief Shape af an area light external data
enum LightAreaShape {
    LIGHT_SHAPE_NONE,
    LIGHT_SHAPE_RECT,
    LIGHT_SHAPE_DISK,
};

#define EXT_ID_LIGHT 3 
typedef struct Light light;

void scobjAddLighExtData_Directional(sc_obj* source, vec3 color);
void scobjAddLighExtData_Point(sc_obj* source, vec3 color, float fallOff, float radius);
void scobjAddLighExtData_Spot(sc_obj* source, vec3 color, float fallOff, float radius, float angle);

/// @brief Get the scene object owner of this light
/// @param l The light
/// @return The owning scene object
sc_obj* lightGetScObj(light* l);
/// @brief Set the coolor of a light
/// @param l The light to set
/// @param newColor The new color to apply
void lightSetColor(light* l, vec3 newColor);
/// @brief Get the type of a light
/// @return The type of the light
enum LightType lightGetType(light* l);

#endif