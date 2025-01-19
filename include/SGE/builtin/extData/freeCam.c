#include "freeCam.h"
DEF_EXT_ID_C(free_cam)

typedef struct P_FreeCam {
    free_cam public;
    vec2 rotPos, rotVel;
    vec3 linVel;

    float FOV;
    float zoomLerp;
} p_free_cam;

free_cam* scobjAttachFreeCam(sc_obj* source, float linVel, float rotVel, float linAcc, float rotAcc, float linFric, float linFricFast, float rotFric) {
    p_free_cam* new = (p_free_cam*)malloc(sizeof(p_free_cam));
    new->public.linFricFast = linFricFast; new->public.linVel = linVel; new->public.rotVel = rotVel;
    new->public.linAcc = linAcc; new->public.rotAcc = rotAcc; new->public.linFric = linFric; new->public.rotFric = rotFric;
    
    new->linVel = vec3_zero; 
    new->rotVel = vec2_zero;
    new->FOV = camGetFOV(scobjGetExtData(source, cam));
    new->zoomLerp = new->FOV;
    
    vec3 temp = vec3_zero; 
    quatToVec3_Euler_(&source->transform.rotation, &temp);
    new->rotPos.x = temp.x;
    new->rotPos.y = temp.y;

    scobjAttachExtData(source, free_cam, new);

    return (free_cam*)new;
}

void freeCamUpdate(sc_obj* object) {
    p_free_cam* data = (p_free_cam*)scobjGetExtData(object, free_cam);
    double dt = SL_min(TIME.dt, 1.0 / 30.0);

    vec2 rot = (vec2){
        inputIsKeyDown(SGE_KEY_K) - inputIsKeyDown(SGE_KEY_M),
        inputIsKeyDown(SGE_KEY_O) - inputIsKeyDown(SGE_KEY_L),
    };

    if (rot.x == 0) data->rotVel.x *= 1.0 - data->public.rotFric * dt;
    else if (rot.x > 0) data->rotVel.x = __min(data->public.rotVel, data->rotVel.x + data->public.rotAcc * dt);
    else data->rotVel.x = __max(-data->public.rotVel, data->rotVel.x - data->public.rotAcc * dt);

    if (rot.y == 0) data->rotVel.y *= 1.0 - data->public.rotFric * dt;
    else if (rot.y > 0) data->rotVel.y = __min(data->public.rotVel, data->rotVel.y + data->public.rotAcc * dt);
    else data->rotVel.y = __max(-data->public.rotVel, data->rotVel.y - data->public.rotAcc * dt);

    data->rotPos.x += data->rotVel.x * dt;
    data->rotPos.y += data->rotVel.y * dt;
    if (data->rotPos.y > PI * 0.5) {
        data->rotPos.y = PI * 0.5 - 0.001;
        data->rotVel.y = 0.0;
    }
    else if (data->rotPos.y < -PI * 0.5) {
        data->rotPos.y = -PI * 0.5 + 0.001;
        data->rotVel.y = 0.0;
    }
    setQ_Euler_(&object->transform.rotation, data->rotPos.x, data->rotPos.y, 0);

    vec3 mouvement = (vec3){
        inputIsKeyDown(SGE_KEY_D) - inputIsKeyDown(SGE_KEY_Q),
        inputIsKeyDown(SGE_KEY_SPACE) - inputIsKeyDown(SGE_KEY_CTRL),
        inputIsKeyDown(SGE_KEY_Z) - inputIsKeyDown(SGE_KEY_S),
    };
    quat q = Quat_Euler(data->rotPos.x, 0, 0);
    rot3Q_s(&mouvement, &q);
    bool speedyBoi = inputIsKeyDown(SGE_KEY_SHIFT) && (mouvement.x || mouvement.y || mouvement.z);
    scale3_s(addS3_s(&data->linVel, &mouvement, dt * data->public.linAcc), 1.0 - (speedyBoi ? data->public.linFricFast : data->public.linFric) * dt);
    addS3_s(&object->transform.position, &data->linVel, dt);

    data->zoomLerp = SL_lerp(data->zoomLerp, inputIsKeyDown(SGE_KEY_C) ? 5.0*DEG_TO_RAD : data->FOV * (speedyBoi ? 1.35 : 1.0), dt * 15.0);
    if (REGetRenderCamera(APP->renderEnvironment)) camSetFOV(REGetRenderCamera(APP->renderEnvironment), data->zoomLerp);
}

sc_obj* freeCam_addDefault(vec3 pos, quat rot, float FOV) {
    sc_obj* camera = newSceneObject(pos, rot, vec3_one, NULL, false, &freeCamUpdate);
    scobjAttachCamera(camera, FOV, 0.03, 1000.0, 1.0);
    scobjAttachFreeCam(camera, 2.0, 2.5, 100.0, 30.0, 10.0, 5.0, 35.0);
    return camera;
}

void registerFreeCam() {
    registerExtData(free_cam, &free);
}