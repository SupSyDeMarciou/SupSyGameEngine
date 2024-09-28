#ifndef __SGE_BUILTIN_TRAIL
#define __SGE_BUILTIN_TRAIL

#include <SupSy/SGE.h>

/// @brief Trail renderer external data
typedef struct Trail trail;

#define EXT_ID_TRAIL 103

/// @brief Create a trail renderer
/// @param focusObject The object from which the trail is emitted
/// @param thickness The maximum thickness of the trail (at its base)
/// @param lifetime The time it takes the trail to go from its thickest to flat
/// @param startingColor The color at the base
/// @param endingColor The color at the tail
/// @return The newly created trail object
sc_obj* newTrail(sc_obj* focusObject, float thickness, float lifetime, vec3 startingColor, vec3 endingColor);
/// @brief The the state of the simulation of the trail
/// @param b Wether to simulate the trail
void trailSimulate(bool b);

#endif