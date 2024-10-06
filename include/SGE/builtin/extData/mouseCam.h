#ifndef __SGE_BUILTIN_CAMERA_CONTROLER
#define __SGE_BUILTIN_CAMERA_CONTROLER

#include "../../../SGE.h"

#define EXT_ID_MOUSE_CAM 106
typedef struct MouseCamera mouse_cam;

void scobjAddMouseCamExtData(sc_obj* object, vec2 posSpeed, vec2 rotSpeed, float posAcc, float rotAcc, float posFric, float posFricFAST, float rotFric);
void mouseCamUpdate(sc_obj* cam);

void mouseCamSetMousePos(mouse_cam* mc, vec2 mp);
sc_obj* mouseCam_addDefault(vec3* pos, quat* rot, float FOV, bool freeTransformData);

#endif