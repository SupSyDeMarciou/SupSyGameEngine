#ifndef __SGE_BUILTIN_MOUSE_CAM_H__
#define __SGE_BUILTIN_MOUSE_CAM_H__

#include "../../../SGE.h"

typedef struct MouseCamera mouse_cam;
DEF_EXT_ID(mouse_cam)

void scobjAttachMouseCam(sc_obj* object, vec2 posSpeed, vec2 rotSpeed, float posAcc, float rotAcc, float posFric, float posFricFAST, float rotFric);
void mouseCamUpdate(sc_obj* cam);

void mouseCamSetMousePos(mouse_cam* mc, vec2 mp);
sc_obj* mouseCam_addDefault(vec3 pos, quat rot, float FOV);

/// @brief Register the mouseCam external data block
void registerMouseCam();

#endif