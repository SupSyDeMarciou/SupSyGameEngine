#include "rayTracer.h"

static uint pathIdx = 1;
static uvec3 groups;
static comp_shader raytrace = -1;
static texture2D* RTresult;
static shader_buff_uni* sceneBuffer;
static sbu_bp* sceneBufferBP;

// Raytracing scene object
typedef struct RTSObject {
    vec4 position;
    vec4 scale;
    uvec4 materialIndex;
} rt_obj;
// Raytracing material
typedef struct RTMaterial {
    vec3 albedo;       
    float roughness;   
    vec4 specular;     
    vec4 emission;     
    float transmission;
    float IOR;         
    vec2 pad;
} rt_mat;

// Raytracing scene
static struct RTScene {
    rt_obj objects[32];
    rt_mat materials[32];
} SCENE = {0};

static uint OBJECT_COUNT = 0;
static uint MATERIALS_COUNT = 0;

bool initializeRayTracer(texture2D* result, uint maxBounces, uint maxTracesPerFrame, int totalSamples) {
    if (raytrace != -1) return false;
    sceneBuffer = newShaderBufferUniform();
    sceneBufferBP = SBUAttachBindingPoint(sceneBuffer, "Scene", 3, sizeof(SCENE));
    SBUFinalize(sceneBuffer);

    raytrace = createComputeShader("trace.cs");
    shaderAttachUniformBufferBP(raytrace, SGE_SBU_BP_Camera);
    shaderAttachUniformBufferBP(raytrace, SGE_SBU_BP_Environment);
    shaderAttachUniformBufferBP(raytrace, sceneBufferBP);

    RTresult = result;
    uvec2 resSize = tex2DGetSize(result);
    groups = uvec2To3_z(addu2(divu2(resSize, Uvec2(8, 8)), modu2(resSize, Uvec2(8, 8))), 1);

    shaderSetFloat4(raytrace, "u_ImageSize", resSize.x, resSize.y, resSize.x / (float)resSize.y, resSize.y / (float)resSize.x);
    shaderSetUint(raytrace, "u_MaxBounces", maxBounces);
    shaderSetUint(raytrace, "u_MaxTracesPerFrame", maxTracesPerFrame);
    shaderSetUint(raytrace, "u_TotalSamples", totalSamples > 0 ? totalSamples : -1);
    SL_randSeed(0);
    return true;
}

void raytracerSetNbBounces(uint n) {
    shaderSetUint(raytrace, "u_MaxBounces", n);
}
void raytracerSetNbTraces(uint n) {
    shaderSetUint(raytrace, "u_MaxTracesPerFrame", n);
}
uint raytracerGetCompletion() {
    return pathIdx;
}

uint raytracerAddObject(vec3 position, float radius, uint materialIdx) {
    SCENE.objects[OBJECT_COUNT].position = vec3To4_w(position, 1);
    SCENE.objects[OBJECT_COUNT].scale = vec3To4_w(vec3One(radius), 1);
    SCENE.objects[OBJECT_COUNT].materialIndex = uvec4One(materialIdx);
    return OBJECT_COUNT++;
}
uint raytracerAddMaterial(vec3 albedo, float rougness, vec3 specularCol, float specularProb, vec3 emission, float emissionStrength, float transmission, float IOR) {
    SCENE.materials[MATERIALS_COUNT].albedo = albedo;
    SCENE.materials[MATERIALS_COUNT].roughness = rougness;
    SCENE.materials[MATERIALS_COUNT].specular = vec3To4_w(specularCol, specularProb);
    SCENE.materials[MATERIALS_COUNT].emission = vec3To4_w(emission, emissionStrength);
    SCENE.materials[MATERIALS_COUNT].transmission = transmission;
    SCENE.materials[MATERIALS_COUNT].IOR = IOR;
    return MATERIALS_COUNT++;
}
material* materialFromRTMat(shader surfaceShader, uint rtMat) {
    rt_mat* m = SCENE.materials + rtMat;
    return newMaterial_PBR(surfaceShader, vec3To4_w(m->albedo, 1), m->roughness, 1.0, m->specular.w, scale3(Vec3(XPD_VEC3(m->emission)), m->emission.w));
}

void raytracerSendScene() {
    SBUSetData(sceneBuffer, &SCENE);
    shaderSetUint(raytrace, "u_NbObj", OBJECT_COUNT);
}

void raytracerRender() {
    cam* camData = REGetRenderCamera(APP->renderEnvironment);
    shaderSetFloat(raytrace, "u_CamDepth", 1.0 / tan(camGetFOV(camData) * 0.5));
    shaderSetUint(raytrace, "u_PathIdx", pathIdx++);
    shaderSetUint(raytrace, "u_Seed", SL_randU32());

    shaderUse(raytrace);
    glBindImageTexture(0, tex2DGetGlID(RTresult), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
    compShaderDispatch(raytrace, XPD_VEC3(groups));
}
void raytracerRestart() {
    pathIdx = 1;
}