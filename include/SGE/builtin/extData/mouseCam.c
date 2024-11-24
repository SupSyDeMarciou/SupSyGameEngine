#include "mouseCam.h"
DEF_EXT_ID_C(mouse_cam)

typedef struct MouseCamera {
    vec2 lastMousePos;
    vec2 rot;

    vec2 linSpeed;
    vec2 rotSpeed;

    vec3 linVel;
    float linFric;
    float linFricFast;
    float linAcc;

    float FOV;
    float zoomLerp;
} mouse_cam;

mouse_cam* scobjAttachMouseCam(sc_obj* object, vec2 linVel, vec2 rotVel, float linAcc, float linFric, float linFricFast) {
    mouse_cam* new = malloc(sizeof(mouse_cam));
    new->lastMousePos = inputGetMousePos();
    vec3 euler; quatToVec3_Euler_(&object->transform.rotation, &euler);
    new->rot.x = euler.x; new->rot.y = euler.y;
    new->linSpeed = linVel;
    new->rotSpeed = rotVel;
    new->linFric = linFric;
    new->linFricFast = linFricFast;
    new->linAcc = linAcc;
    new->FOV = camGetFOV(scobjGetExtData(object, cam));
    new->zoomLerp = new->FOV;

    new->linVel = vec3_zero;
    scobjAddExtData(object, mouse_cam, new);
    return new;
}

void mouseCamUpdate(sc_obj* cam) {
    mouse_cam* data = scobjGetExtData(cam, mouse_cam);
    trsfrm* t = &cam->transform;
    double dt = SL_min(TIME.dt, 1.0 / 30.0);

    vec2 rot = sub2(data->lastMousePos, inputGetMousePos());

    addSM2_s(&data->rot, &rot, TIME.dt, &data->rotSpeed);
    if (data->rot.y > PI * 0.5) data->rot.y = PI * 0.5 - 0.001;
    else if (data->rot.y < -PI * 0.5) data->rot.y = -PI * 0.5 + 0.001;
    setQ_Euler_(&t->rotation, data->rot.x, data->rot.y, 0);

    vec3 mouvement = (vec3){
        inputIsKeyDown(SGE_KEY_D) - inputIsKeyDown(SGE_KEY_Q),
        inputIsKeyDown(SGE_KEY_SPACE) - inputIsKeyDown(SGE_KEY_CTRL),
        inputIsKeyDown(SGE_KEY_Z) - inputIsKeyDown(SGE_KEY_S),
    };
    quat q = Quat_Euler(data->rot.x, 0, 0);
    rot3Q_s(&mouvement, &q);
    bool speedyBoi = inputIsKeyDown(SGE_KEY_SHIFT) && (mouvement.x || mouvement.y || mouvement.z);
    scale3_s(addS3_s(&data->linVel, &mouvement, dt * data->linAcc), 1.0 - (speedyBoi ? data->linFricFast : data->linFric) * dt);
    addS3_s(&t->position, &data->linVel, dt);

    data->zoomLerp = SL_lerp(data->zoomLerp, inputIsKeyDown(SGE_KEY_C) ? 5.0*DEG_TO_RAD : data->FOV * (speedyBoi ? 1.35 : 1.0), dt * 15.0);
    if (REGetRenderCamera(APP->renderEnvironment)) camSetFOV(REGetRenderCamera(APP->renderEnvironment), data->zoomLerp);
    
    data->lastMousePos = inputGetMousePos();
}

void mouseCamSetMousePos(mouse_cam* mc, vec2 mp) {
    mc->lastMousePos = mp;
}

sc_obj* mouseCam_addDefault(vec3 pos, quat rot, float FOV) {
    sc_obj* camera = newSceneObject(pos, rot, vec3_one, NULL, false, &mouseCamUpdate);
    scobjAttachCamera(camera, FOV, 0.03, 1000.0, 1.0);
    scobjAttachMouseCam(camera, Vec2(1.5, 2.0), Vec2(0.3, 0.25), 100.0, 10.0, 5.0);
    return camera;
}

void registerMouseCam() {
    extDataRegister(&EXT_ID(mouse_cam), &free);
}