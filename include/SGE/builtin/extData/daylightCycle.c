#include "daylightCycle.h"
DEF_EXT_ID_C(daylight_cycle)

void updateDaylightCycle(sc_obj* object) {
    daylight_cycle* data = scobjGetExtData(object, daylight_cycle); // Retrieve the external data

    data->currentAngle += data->rotationSpeed * TIME.dt; // Increase angle with constant velocity
    setQ_AngleAxis_(&object->transform.rotation, data->currentAngle, &data->rotationAxis); // Apply rotation to the sun
}

void scobjAttachDaylightCycle(sc_obj* source, vec3 rotationAxis, double revolutionTime, double initialAngle) {
    daylight_cycle* new = (daylight_cycle*)malloc(sizeof(daylight_cycle));
    
    new->rotationAxis = rotationAxis;
    new->rotationSpeed = TAU / revolutionTime;
    new->currentAngle = initialAngle;

    scobjAttachExtData(source, daylight_cycle, new); // Add the external data to the source
}

sc_obj* daylightCycle_addDefault(vec3 rotationAxis, double revolutionTime, double initialAngle) {
    sc_obj* sunTransform = newSceneObject(vec3_zero, quat_identity, vec3_one, NULL, false, &updateDaylightCycle);
    scobjAttachDaylightCycle(sunTransform, rotationAxis, revolutionTime, initialAngle);
    return sunTransform;
}

void registerDaylightCycle() {
    registerExtData(daylight_cycle, &free);
}