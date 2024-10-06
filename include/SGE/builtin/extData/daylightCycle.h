#ifndef __SGE_BUILTIN_DAYLIGHT_CYCLE
#define __SGE_BUILTIN_DAYLIGHT_CYCLE

#include "../../../SGE.h"

#define EXT_ID_DAYLIGHT_CYCLE 102
/// @brief Daylight cycle externa data structure
typedef struct DaylightCycleExternalData {
    vec3 rotationAxis;
    double currentAngle;
    double rotationSpeed;
} daylight_cycle_ext_data;

/// @brief Add a new free cam external data block
/// @param source The scene object to which this data is to be added
/// @param rotationAxis The axis arround which to rotate the sun
/// @param revolutionTime The time it takes to make a full revolution in seconds
/// @param initialAngle The initial angle of the sun in the sky
void scobjAddDaylightCycleTransformExternalData(sc_obj* source, vec3 rotationAxis, double revolutionTime, double initialAngle);

/// @brief The update function associated with 'FreeCamExternalData'
/// @param object The object on which to call the update function
void updateDaylightCycle(sc_obj* object);

/// @brief Add the default daylightCycle sun to the scene
/// @param revolutionTime The time in seconds for a full daylight cycle to occur
/// @param rotationAxis The axis around which revolves the sun
/// @param initialAngle The initial angle of the sun in the sky
/// @return The newly created and added daylightCycle dummy object
sc_obj* daylightCycle_addDefault(vec3 rotationAxis, double revolutionTime, double initialAngle);

#endif