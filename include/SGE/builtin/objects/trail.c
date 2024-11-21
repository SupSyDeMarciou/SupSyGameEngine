#include "trail.h"
DEF_EXT_ID_C(trail)

static shader trailShader = 0;
static bool simulate = true;
#define NB_PARTS 256
struct Trail {
    sc_obj* source;

    sc_obj* focusObject;
    float thickness, lifetime;

    vec3 positions[NB_PARTS + 1];
    uint vert;
    double lastTime;
};

typedef struct TrailMaterial {
    vec3 startingColor, endingColor;
} trail_mat;

static void trailMat_sendData(material* mat) {
    trail_mat* trailMat = mat->materialData;
    shaderSetVec3(trailShader, "u_ColorStart", &trailMat->startingColor);
    shaderSetVec3(trailShader, "u_ColorEnd", &trailMat->endingColor);
}

static void trail_onBeforeRender(sc_obj* self) {
    if (!simulate) return;
    render_obj* ro = scobjGetExtData(self, render_obj);
    trail* tr = scobjGetExtData(self, trail);
    mesh* m = ro->mesh;

    cam* c = REGetRenderCamera();

    vec3 wpos = scobjWorldPos(tr->focusObject);
    quat wrot = camGetScObj(c)->transform.rotation;
    tr->positions[tr->vert] = wpos;
    // tranposeQs(&wrot);
    vec3 wup = vec3_forward; 
    // rot3Qs(&wup, &wrot);


    for (uint i = 0; i < NB_PARTS + 1; i++) {
        uint j = (i + tr->vert + NB_PARTS + 2) % (NB_PARTS + 1);

        float factor = i / (float)(NB_PARTS + 1);
        vec3 lup;
        if (j == 0 || j == NB_PARTS) lup = add3(tr->positions[NB_PARTS], tr->positions[1]);
        else lup = add3(tr->positions[j - 1], tr->positions[j + 1]);
        
        // vec3 offset; scale3ps(normalize3ps(projPlane3p(&lup, &wup, &offset)), factor * factor * tr->thickness);
        vec3 offset; scale3_s(norm3_s(vec3ProjPlane_(&lup, &wup, &offset)), factor * factor * tr->thickness);
        // rot3Qs(&lup, &wrot);

        // vec3 offset = scale3(lup, factor * factor * tr->thickness);
        
        m->vertices[i * 2 + 0].position = add3(tr->positions[j], offset);
        m->vertices[i * 2 + 0].uv = Vec2(factor, 1);
        m->vertices[i * 2 + 1].position = sub3(tr->positions[j], offset);
        m->vertices[i * 2 + 1].uv = Vec2(factor, 0);
    }


    if (TIME.timeAtFrameStart - tr->lastTime >= tr->lifetime) {
        tr->lastTime = TIME.timeAtFrameStart;
        tr->vert++;
        if (tr->vert == NB_PARTS + 1) tr->vert = 0;
    } 

    meshMarkUnsynced(m);
}

sc_obj* newTrail(sc_obj* focusObject, float thickness, float lifetime, vec3 startingColor, vec3 endingColor) {
    trail* new = (trail*)malloc(sizeof(trail));

    new->focusObject = focusObject;
    new->thickness = thickness;
    new->lifetime = lifetime;

    new->lastTime = TIME.timeAtFrameStart;
    new->vert = 0;

    new->source = newSceneObject(vec3_zero, quat_identity, vec3_one, NULL, false, NULL);
    if (!trailShader) trailShader = createSurfaceShader("!builtin/objects/trail.fs");
    trail_mat* trailMat = malloc(sizeof(trail_mat));
    trailMat->startingColor = startingColor;
    trailMat->endingColor = endingColor;

    vertex vertices[NB_PARTS * 2 + 2] = {0};
    triangle triangles[NB_PARTS * 2] = {0};
    vec3 base = scobjWorldPos(focusObject);
    for (uint i = 0; i < NB_PARTS + 1; i++) {
        float fact = i / (float)(NB_PARTS + 1);
        uint I = i + i;
        vertices[I + 0] = (vertex){
            .position = base,
            .color = startingColor,
            .normal = vec3_zero,
            .uv = Vec2(fact, 1)
        };
        vertices[I + 1] = (vertex){
            .position = base,
            .color = startingColor,
            .normal = vec3_zero,
            .uv = Vec2(fact, 0)
        };
        if (i == NB_PARTS) break;

        triangles[I + 0] = (triangle){I, I + 1, I + 2};
        triangles[I + 1] = (triangle){I + 1, I + 2, I + 3};
    }
    mesh* trailMesh = newMesh(2 * NB_PARTS + 2, vertices, 2 * NB_PARTS, triangles, 1, NULL);
    
    scobjAttachRenderObject_SingleMat(new->source, trailMesh, newMaterial(trailShader, trailMat_sendData, trailMat, free), true, RENDER_CULL_NONE, false, trail_onBeforeRender, NULL);
    scobjAddExtData(new->source, trail, new);

    return new->source;
}

void trailSimulate(bool b) {
    simulate = b;
}

void registerTrail() {
    extDataRegister(&EXT_ID(trail), &free);
}