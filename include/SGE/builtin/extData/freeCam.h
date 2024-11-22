#ifndef __SGE_BUILTIN_FREE_CAM_H__
#define __SGE_BUILTIN_FREE_CAM_H__

#include "../../../SGE.h"

/// @brief Free camera external data structure
typedef struct FreeCam {
    float linVel, rotVel;

    float linAcc, rotAcc;
    float linFric, linFricFast, rotFric;
} free_cam;
DEF_EXT_ID(free_cam)

/// @brief Add a new freeCam external data block
/// @param source The scene object to which this data is to be added
/// @param linVel The maximum linear velocity
/// @param rotVel The maximum angular velocity
/// @param linAcc The linear acceleration
/// @param rotAcc The rotational acceleration
/// @param linFric The linear friction
/// @param linFricFast The linear friction when speeding
/// @param rotFric The rotational friction
free_cam* scobjAttachFreeCam(sc_obj* source, float linVel, float rotVel, float linAcc, float rotAcc, float linFric, float linFricFast, float rotFric);
/// @brief The update function associated with 'free_cam'
/// @param object The object on which to call the update function
void freeCamUpdate(sc_obj* object);

/// @brief Add the default freeCam camera to the scene
/// @param pos Initial position of the camera
/// @param rot Initial rotation of the camera
/// @param FOV Vertical FOV
/// @return The newly created and added camera
sc_obj* freeCam_addDefault(vec3 pos, quat rot, float FOV);

/// @brief Register the freeCam external data block
void registerFreeCam();

#endif