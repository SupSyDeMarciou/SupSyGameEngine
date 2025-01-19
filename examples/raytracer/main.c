#define SGE_MAIN
#define SGE_BASE_HEIGHT 2160
#define SGE_BASE_WIDTH 3840

#include <SupSy/SGE.h>
#include <SupSy/SGE/builtin/extData/freeCam.h>
#include <SupSy/SGE/builtin/extData/daylightCycle.h>

#include "raytracer.h"

// Add sphere to both SGE rendering pipeline and RayTracing pipeline
sc_obj* newSphere(shader surfaceShader, mesh* sphereMesh, vec3 pos, float radius, uint mat) {
    sc_obj* sphere = newSceneObject(pos, quat_identity, vec3One(2.0 * radius), NULL, true, NULL);
    scobjAttachRenderObject_SingleMat(sphere, sphereMesh, materialFromRTMat(surfaceShader, mat), true, RENDER_CULL_NONE, true, false, NULL, NULL);
    raytracerAddObject(pos, radius,  mat);
}

// To render the box behind the text
void boxRender(shader boxShader, vec2 pos, vec2 scale, vec4 color) {
    shaderSetVec2(boxShader, "u_Position", &pos);
    shaderSetVec2(boxShader, "u_Scale", &scale);
    shaderSetVec4(boxShader, "u_Color", &color);
    shaderSetFloat(boxShader, "u_Ratio", REGetScreenRatio());
    RERenderQuad(APP->renderEnvironment, boxShader);
}

int main() {

    // Initialize and ragister
    initializeApp("RayTracer");
    registerFreeCam();
    registerDaylightCycle();

    frame_buffer* fbRT = newFrameBuffer(Uvec2(SGE_BASE_WIDTH, SGE_BASE_HEIGHT));
    FBAttachColor(fbRT, tex2DGenFormat(TEX_COL_RGBA, TEX_BIT_DEPTH_16, TEX_FORM_FLOAT));
    FBFailIfNotComplete(fbRT);

    const uint totalSamples = 2048;
    initializeRayTracer(FBGetColor(fbRT, 0), 5, 15, totalSamples);

    // Create scene
    shader surfaceShader = createSurfaceShader("!builtin//shaders/pbr.fs");
    mesh* sphereMesh = meshUnitUVShpere(128, 64);

    uint m0 = raytracerAddMaterial(Vec3(0.5, 0.2, 1.0), 0.01, Vec3(0.5, 0.2, 1.0), 1.0, Vec3(0, 0, 0), 0, 0, 1);
    newSphere(surfaceShader, sphereMesh, Vec3(0.5, -0.01, 0), 1.0, m0);

    uint m1 = raytracerAddMaterial(Vec3(0.01, 0.01, 0.01), 0.9, Vec3(0.01, 0.01, 0.01), 0.1, Vec3(0, 0, 0), 0, 0, 1);
    newSphere(surfaceShader, sphereMesh, Vec3(0, -101, 0), 100.0, m1);

    uint m2 = raytracerAddMaterial(Vec3(0.1, 0.5, 1.0), 0.01, Vec3(0.1, 0.5, 1.0), 0.1, Vec3(0, 0, 0), 0, 0, 1);
    newSphere(surfaceShader, sphereMesh, Vec3(0.0, 0.425, 3.0), 1.5, m2);

    uint m3 = raytracerAddMaterial(Vec3(0, 0, 0), 0, Vec3(0, 0, 0), 0, Vec3(1, 0, 1), 20, 0, 1);
    newSphere(surfaceShader, sphereMesh, Vec3(0, -.55, -2), 0.5, m3);

    uint m4 = raytracerAddMaterial(Vec3(1, 1, 1), 0, Vec3(1, 1, 1), 0.02, vec3_zero, 0, 1.0, 1.45);
    newSphere(surfaceShader, sphereMesh, Vec3(-4, 0.725, 6), 2.0, m4);

    raytracerSendScene();

    // Create camera
    sc_obj* cameraObj = freeCam_addDefault(Vec3(-2.0, 0.0, -4.0), quat_identity, 60.0*DEG_TO_RAD);
    // sc_obj* cameraObj = mouseCam_addDefault(vec3_zero, quat_identity, 60.0*DEG_TO_RAD);
    cam* camera = scobjGetExtData(cameraObj, cam);
    RESetRenderCamera(camera);

    // Create Sun
    sc_obj* sun = newSceneObject(vec3_zero, quat_identity, vec3_one, NULL, false, updateDaylightCycle);
    light* sunLight = scobjAttachLight_Directional(sun, scale3(Vec3(1, 1, 0.9), 70.0));
    scobjAttachDaylightCycle(sun, vec3Unit(1, 0, 0), 120, PI);
    simpleSkySetSun(REGetBackground(), scobjGetExtData(sun, light));

    // Create text
    font* f = fontImport("!builtin/fonts/CASCADIACODE.ttf");
    text* info = newText_Base(f, L" Rendering mode : [---] | Accumulation : ------ / ------\n Rays per pass : ------ | Max bounces : ------", Vec2(50, 1), TEXT_ANCHOR_LEFT | TEXT_ANCHOR_TOP, 0.02, Vec4(0.5, 0.5, 0.5, 1.0));
    shader text_shader = createShader("!render/shaders/text2D.vs", "!render/shaders/text.fs", NULL);
    shader box_shader = createShader("box.vs", "box.fs", NULL);

    frame_buffer* fb0 = REGetOutputFB(APP->renderEnvironment);
    frame_buffer* fb1 = newFrameBuffer(Uvec2(SGE_BASE_WIDTH, SGE_BASE_HEIGHT));
    FBAttachColor(fb1, tex2DGenFormat(TEX_COL_RGBA, TEX_BIT_DEPTH_16, TEX_FORM_FLOAT));
    FBFailIfNotComplete(fb1);

    // State
    bool RTXAccumulating = false;
    bool RTX = false;
    uint raysPerPass = 25;
    uint maxBounces = 6;
    float focalDistance = 5.0;
    float focalDispersion = 0.15;
    double startAccumulationTime = 0;
    double totalRenderTime = 0;
    while (!appShouldClose()) {
        startFrameUpdate(); // START FRAME
        if (inputIsKeyPressed(SGE_KEY_ESCAPE)) appSetShouldClose(true);
        
        // Switch render modes with TAB
        if (inputIsKeyPressed(SGE_KEY_TAB) && !RTXAccumulating) RTX = !RTX;
        // When raytracing, activate ACCUMULATION with ENTER
        if (inputIsKeyPressed(SGE_KEY_ENTER) && RTX) {
            RTXAccumulating = !RTXAccumulating;
            if (RTXAccumulating) {
                maxBounces = 15;
                raysPerPass = 8;
            }
            else {
                maxBounces = 6;
                raysPerPass = 2;
            }

            raytracerSetNbBounces(maxBounces);
            raytracerSetNbTraces(raysPerPass);
        }

        // When doing accumulation, stop scene updates so the scene stays fixed
        if (!RTXAccumulating) {
            focalDistance += (inputIsKeyDown(SGE_KEY_UP) - inputIsKeyDown(SGE_KEY_DOWN)) * TIME.dt * 5.0;
            focalDispersion += (inputIsKeyDown(SGE_KEY_RIGHT) - inputIsKeyDown(SGE_KEY_LEFT)) * TIME.dt;
            if (focalDispersion < 0.0) focalDispersion = 0.0;

            raytracerSetFocalDistance(focalDistance);
            raytracerSetFocalDispersion(focalDispersion);

            sceneUpdate(APP->scene);
            startAccumulationTime = TIME.timeAtFrameStart;
        }

        if (RTX) {
            if (!RTXAccumulating) {
                // If not accumulating, update environment
                simpleSkyRender();
                REUpdateGPUEnvironmentBuffer();
                REUpdateGPUCameraBuffer(camera);
                raytracerRestart();
            }
            // Stop accumulating when reaching total samples count
            if (raytracerGetCompletion() < totalSamples) raytracerRender();
            blitHdrToLdrFB(fbRT, fb1);
        }
        else {
            RERenderSceneFB(fb0);
            blitHdrToLdrFB(fb0, fb1);
        }

        // Calculate debug info
        if (RTXAccumulating) {
            float passed = TIME.timeAtFrameStart - startAccumulationTime; 
            float rem = passed * (totalSamples / (float)raytracerGetCompletion() - 1.0);
            if (raytracerGetCompletion() == totalSamples - 1) totalRenderTime = passed;
            if (raytracerGetCompletion() >= totalSamples)
                _swprintf(info->text, L" Rendering mode : [%ls] | Accumulation : %u / %u | Rays per pass : %u | Max bounces : %u | Render completed in %fs", 
                RTX ? L"RTX" : L"SGE", raytracerGetCompletion(), totalSamples, raysPerPass, maxBounces, totalRenderTime);
            else 
                _swprintf(info->text, L" Rendering mode : [%ls] | Accumulation : %u / %u | Rays per pass : %u | Max bounces : %u | Time passed : %fs | Remaining : %fs", 
                RTX ? L"RTX" : L"SGE", raytracerGetCompletion(), totalSamples, raysPerPass, maxBounces, passed, rem);
        }
        else _swprintf(info->text, L" Rendering mode : [%ls] | Accumulation : %u / %u | Rays per pass : %u | Max bounces : %u", RTX ? L"RTX" : L"SGE", raytracerGetCompletion(), totalSamples, raysPerPass, maxBounces);
        textSetString(info, info->text);

        // Show debug info
        vec2 infoPos = Vec2(-1.0, 1.0 / REGetScreenRatio());
        vec2 scale = scale2(info->trueBoxSize, 0.5);
        vec2 pos = add2(infoPos, Vec2(scale.x, -scale.y));
        boxRender(box_shader, pos, scale, Vec4(0, 0, 0, 0.5));
        textRender(info, text_shader, infoPos);

        blitToScreenFB(fb1);
        endFrameUpdate(); // END FRAME
    }

    destroyApp();
    return 0;
}