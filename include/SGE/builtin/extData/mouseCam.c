#include "mouseCam.h"

typedef struct MouseCamera {
    vec2 lastMousePos;
    vec2 rot;

    vec2 linSpeed;
    vec2 rotSpeed;

    vec3 linVel;
    vec2 rotVel;
    float linFric;
    float linFricFast;
    float rotFric;
    float linAcc;
    float rotAcc;

    float FOV;
    float zoomLerp;
} cam_contr_ext_data;

void scobjAddMouseCamExtData(sc_obj* object, vec2 linSpeed, vec2 rotSpeed, float linAcc, float rotAcc, float linFric, float linFricFast, float rotFric) {
    cam_contr_ext_data* new = malloc(sizeof(cam_contr_ext_data));
    new->lastMousePos = Vec2(0, 0);
    vec3 euler; quatToVec3_Euler_(&object->transform.rotation, &euler);
    new->rot.x = euler.y; new->rot.y = euler.x;
    new->linSpeed = linSpeed;
    new->rotSpeed = rotSpeed;
    new->linFric = linFric;
    new->linFricFast = linFricFast;
    new->rotFric = rotFric;
    new->linAcc = linAcc;
    new->rotAcc = rotAcc;
    new->FOV = camGetFOV((cam*)scobjGetExtData(object, EXT_ID_CAMERA));
    new->zoomLerp = new->FOV;
    new->linVel = vec3_zero;
    new->rotVel = vec2_zero;
    scobjAddExtDataP(object, EXT_ID_MOUSE_CAM, new, free);
}

void mouseCamUpdate(sc_obj* cam) {
    cam_contr_ext_data* data = scobjGetExtData(cam, EXT_ID_MOUSE_CAM);
    trsfrm* t = &cam->transform;
    double dt = SL_min(TIME.dt, 1.0 / 30.0);

    vec2 rot = sub2(inputGetMousePos(), data->lastMousePos);

    data->rotVel.x = SL_max(-data->rotSpeed.x, SL_min(data->rotSpeed.x, data->rotVel.x + data->rotAcc * dt * rot.x));
    data->rotVel.y = SL_max(-data->rotSpeed.y, SL_min(data->rotSpeed.y, data->rotVel.y + data->rotAcc * dt * rot.y));
    data->rotVel.x *= 1.0 - data->rotFric * dt;
    data->rotVel.y *= 1.0 - data->rotFric * dt;

    data->rot.x += data->rotVel.x * dt;
    data->rot.y += data->rotVel.y * dt;
    if (data->rot.y > PI * 0.5) {
        data->rot.y = PI * 0.5 - 0.001;
        data->rotVel.y = 0.0;
    }
    else if (data->rot.y < -PI * 0.5) {
        data->rot.y = -PI * 0.5 + 0.001;
        data->rotVel.y = 0.0;
    }
    setQ_Euler_(&t->rotation, data->rot.y, data->rot.x, 0);

    vec3 mouvement = (vec3){
        inputIsKeyDown(SGE_KEY_D) - inputIsKeyDown(SGE_KEY_Q),
        inputIsKeyDown(SGE_KEY_SPACE) - inputIsKeyDown(SGE_KEY_CTRL),
        inputIsKeyDown(SGE_KEY_Z) - inputIsKeyDown(SGE_KEY_S),
    };
    quat q = Quat_Euler(0, data->rot.x, 0);
    rot3Q_s(&mouvement, &q);
    bool speedyBoi = inputIsKeyDown(SGE_KEY_SHIFT) && (mouvement.x || mouvement.y || mouvement.z);
    scale3_s(addS3_s(&data->linVel, &mouvement, dt * data->linAcc), 1.0 - (speedyBoi ? data->linFricFast : data->linFric) * dt);
    addS3_s(&t->position, &data->linVel, dt);

    data->zoomLerp = SL_lerp(data->zoomLerp, inputIsKeyDown(SGE_KEY_C) ? 5.0 : data->FOV * (speedyBoi ? 1.35 : 1.0), dt * 15.0);
    if (REGetRenderCamera(APP->renderEnvironment)) camSetFOV(REGetRenderCamera(APP->renderEnvironment), data->zoomLerp);
    
    data->lastMousePos = inputGetMousePos();
}

void mouseCamSetMousePos(mouse_cam* mc, vec2 mp) {
    mc->lastMousePos = mp;
}

sc_obj* mouseCam_addDefault(vec3* pos, quat* rot, float FOV, bool freeTransformData) {
    sc_obj* camera = newCamera(pos, rot, NULL, freeTransformData, FOV, 0.03, 1000.0, SGE_BASE_WIDTH / (float)SGE_BASE_WIDTH);
    scobjAddMouseCamExtData(camera, Vec2(1.5, 2.0), Vec2(100.0, 100.0), 100.0, 30.0, 10.0, 5.0, 35.0);
    camera->update = (func_update*)&mouseCamUpdate;
    return camera;
}