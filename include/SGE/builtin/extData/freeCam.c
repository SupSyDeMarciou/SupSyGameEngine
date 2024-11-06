#include "freeCam.h"

typedef struct P_FreeCamExternalData {
    free_cam_ext_data public;
    vec2 rotPos, rotVel;
    vec3 linVel;

    float FOV;
    float zoomLerp;
} p_free_cam_ext_data;

void scobjAddFreeCamExtData(sc_obj* object, float linVel, float rotVel, float linAcc, float rotAcc, float linFric, float linFricFast, float rotFric) {
    p_free_cam_ext_data* new = (p_free_cam_ext_data*)malloc(sizeof(p_free_cam_ext_data));
    new->public.linFricFast = linFricFast; new->public.linVel = linVel; new->public.rotVel = rotVel;
    new->public.linAcc = linAcc; new->public.rotAcc = rotAcc; new->public.linFric = linFric; new->public.rotFric = rotFric;
    
    new->linVel = vec3_zero; 
    new->rotVel = vec2_zero;
    new->FOV = camGetFOV((cam*)scobjGetExtData(object, EXT_ID_CAMERA));
    new->zoomLerp = new->FOV;
    
    vec3 temp = vec3_zero; 
    quatToVec3_Euler_(&object->transform.rotation, &temp);
    new->rotPos.x = temp.y;
    new->rotPos.y = temp.x;

    scobjAddExtDataP(object, EXT_ID_FREE_CAM, new, (func_free*)&free);
}

void updateFreeCam(sc_obj* object) {
    p_free_cam_ext_data* data = scobjGetExtData(object, EXT_ID_FREE_CAM);
    double dt = SL_min(TIME.dt, 1.0 / 30.0);

    vec2 rot = (vec2){
        inputIsKeyDown(SGE_KEY_M) - inputIsKeyDown(SGE_KEY_K),
        inputIsKeyDown(SGE_KEY_L) - inputIsKeyDown(SGE_KEY_O),
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
    setQ_Euler_(&object->transform.rotation, data->rotPos.y, data->rotPos.x, 0);

    vec3 mouvement = (vec3){
        inputIsKeyDown(SGE_KEY_D) - inputIsKeyDown(SGE_KEY_Q),
        inputIsKeyDown(SGE_KEY_SPACE) - inputIsKeyDown(SGE_KEY_CTRL),
        inputIsKeyDown(SGE_KEY_Z) - inputIsKeyDown(SGE_KEY_S),
    };
    quat q = Quat_Euler(0, data->rotPos.x, 0);
    rot3Q_s(&mouvement, &q);
    bool speedyBoi = inputIsKeyDown(SGE_KEY_SHIFT) && (mouvement.x || mouvement.y || mouvement.z);
    scale3_s(addS3_s(&data->linVel, &mouvement, dt * data->public.linAcc), 1.0 - (speedyBoi ? data->public.linFricFast : data->public.linFric) * dt);
    addS3_s(&object->transform.position, &data->linVel, dt);

    data->zoomLerp = SL_lerp(data->zoomLerp, inputIsKeyDown(SGE_KEY_C) ? 5.0 : data->FOV * (speedyBoi ? 1.35 : 1.0), dt * 15.0);
    if (REGetRenderCamera(APP->renderEnvironment)) camSetFOV(REGetRenderCamera(APP->renderEnvironment), data->zoomLerp);
}

sc_obj* freeCam_addDefault(vec3* pos, quat* rot, float FOV, bool freeTransformData) {
    sc_obj* camera = newCamera(pos, rot, NULL, freeTransformData, FOV, 0.03, 1000.0, SGE_BASE_WIDTH / (float)SGE_BASE_WIDTH);
    scobjAddFreeCamExtData(camera, 2.0, 2.5, 100.0, 30.0, 10.0, 5.0, 35.0);
    camera->update = (func_update*)&updateFreeCam;
    return camera;
}