#include "daylightCycle.h"

void updateDaylightCycle(sc_obj* object) {
    daylight_cycle_ext_data* data = scobjGetExtData(object, EXT_ID_DAYLIGHT_CYCLE); // Retrieve the external data

    data->currentAngle += data->rotationSpeed * TIME.dt; // Increase angle with constant velocity
    setQ_AngleAxis_(&object->transform.rotation, data->currentAngle, &data->rotationAxis); // Apply rotation to the sun
}

void scobjAddDaylightCycleTransformExternalData(sc_obj* source, vec3 rotationAxis, double revolutionTime, double initialAngle) {
    daylight_cycle_ext_data* new = (daylight_cycle_ext_data*)malloc(sizeof(daylight_cycle_ext_data));
    
    new->rotationAxis = rotationAxis;
    new->rotationSpeed = TAU / revolutionTime;
    new->currentAngle = initialAngle;

    scobjAddExtDataP(source, EXT_ID_DAYLIGHT_CYCLE, new, &free); // Add the external data to the source
}

sc_obj* daylightCycle_addDefault(vec3 rotationAxis, double revolutionTime, double initialAngle) {
    sc_obj* sunTransform = newSceneObject((vec3*)&vec3_zero, (quat*)&quat_identity, (vec3*)&vec3_one, false, NULL, false, &updateDaylightCycle);

    scobjAddDaylightCycleTransformExternalData(sunTransform, rotationAxis, revolutionTime, initialAngle);
    sceneAddObject(APP->scene, sunTransform);
    
    return sunTransform;
}