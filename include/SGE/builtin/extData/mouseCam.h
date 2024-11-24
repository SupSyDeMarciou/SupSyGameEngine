#ifndef __SGE_BUILTIN_MOUSE_CAM_H__
#define __SGE_BUILTIN_MOUSE_CAM_H__

#include "../../../SGE.h"

typedef struct MouseCamera mouse_cam;
DEF_EXT_ID(mouse_cam)


/// @brief Add a new mouseCam external data block
/// @param source The scene object to which this data is to be added
/// @param linVel The maximum linear velocity
/// @param rotVel The maximum angular velocity
/// @param linAcc The linear acceleration
/// @param linFric The linear friction
/// @param linFricFast The linear friction when speeding
mouse_cam* scobjAttachMouseCam(sc_obj* object, vec2 linVel, vec2 rotVel, float linAcc, float linFric, float linFricFast);
/// @brief The update function associated with 'mouse_cam'
/// @param object The object on which to call the update function
void mouseCamUpdate(sc_obj* object);

void mouseCamSetMousePos(mouse_cam* mc, vec2 mp);

/// @brief Add the default mouseCam camera to the scene
/// @param pos Initial position of the camera
/// @param rot Initial rotation of the camera
/// @param FOV Vertical FOV
/// @return The newly created and added camera
sc_obj* mouseCam_addDefault(vec3 pos, quat rot, float FOV);

/// @brief Register the mouseCam external data block
void registerMouseCam();

#endif