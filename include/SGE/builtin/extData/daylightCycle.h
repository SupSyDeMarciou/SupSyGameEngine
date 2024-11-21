#ifndef __SGE_BUILTIN_DAYLIGHT_CYCLE_H__
#define __SGE_BUILTIN_DAYLIGHT_CYCLE_H__

#include "../../../SGE.h"

/// @brief Daylight cycle external data structure
typedef struct DaylightCycle {
    vec3 rotationAxis;
    double currentAngle;
    double rotationSpeed;
} daylight_cycle;
DEF_EXT_ID(daylight_cycle)

/// @brief Add a new free cam external data block
/// @param source The scene object to which this data is to be added
/// @param rotationAxis The axis arround which to rotate the sun
/// @param revolutionTime The time it takes to make a full revolution in seconds
/// @param initialAngle The initial angle of the sun in the sky
void scobjAttachDaylightCycle(sc_obj* source, vec3 rotationAxis, double revolutionTime, double initialAngle);

/// @brief The update function associated with 'FreeCamExternalData'
/// @param object The object on which to call the update function
void updateDaylightCycle(sc_obj* object);

/// @brief Add the default daylightCycle sun to the scene
/// @param revolutionTime The time in seconds for a full daylight cycle to occur
/// @param rotationAxis The axis around which revolves the sun
/// @param initialAngle The initial angle of the sun in the sky
/// @return The newly created and added daylightCycle dummy object
sc_obj* daylightCycle_addDefault(vec3 rotationAxis, double revolutionTime, double initialAngle);

/// @brief Register the daylight cycle external data block
void registerDaylightCycle();

#endif