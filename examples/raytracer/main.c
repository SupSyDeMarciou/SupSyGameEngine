#include <SupSy/SGE.h>
#include <SupSy/SGE/builtin/extData/mouseCam.h>
#include <SupSy/SGE/builtin/extData/freeCam.h>
#include <SupSy/SGE/builtin/extData/daylightCycle.h>
#include <SupSy/SGE/builtin/postEffects/bloom.h>

#include "rayTracer.h"

// Add sphere to both SGE rendering pipeline and RayTracing pipeline
sc_obj* newSphere(shader surfaceShader, mesh* sphereMesh, vec3 pos, float radius, uint mat) {
    sc_obj* sphere = newSceneObject(pos, quat_identity, vec3One(2.0 * radius), NULL, true, NULL);
    scobjAttachRenderObject_SingleMat(sphere, sphereMesh, materialFromRTMat(surfaceShader, mat), true, RENDER_CULL_NONE, true, NULL, NULL);
    raytracerAddObject(pos, radius,  mat);
}

int main() {

    // Initialize and ragister
    initializeApp("RayTracer");
    initializeBloom();
    registerFreeCam();
    registerMouseCam();
    registerDaylightCycle();

    frame_buffer* fbRT = newFrameBuffer(Uvec2(SGE_BASE_WIDTH, SGE_BASE_HEIGHT));
    FBAttachColor(fbRT, tex2DGenFormat(TEX_COL_RGBA, TEX_BIT_DEPTH_16, TEX_FORM_FLOAT));
    if (!isFBComplete(fbRT)) failWithError("FBRT not complete", 0);

    initializeRayTracer(FBGetColor(fbRT, 0), 5, 15, -1);

    // Create scene
    shader surfaceShader = createSurfaceShader("!builtin//shaders/pbr.fs");
    mesh* sphereMesh = meshUnitUVShpere(64*2, 32*2);

    uint m0 = raytracerAddMaterial(Vec3(0.5, 0.2, 1.0), 0.01, Vec3(0.5, 0.2, 1.0), 1.0, Vec3(0, 0, 0), 0, 0, 1);
    newSphere(surfaceShader, sphereMesh, Vec3(0.5, 0, 0), 1.0, m0);

    uint m1 = raytracerAddMaterial(Vec3(0.01, 0.01, 0.01), 0.9, Vec3(0.01, 0.01, 0.01), 0.1, Vec3(0, 0, 0), 0, 0, 1);
    newSphere(surfaceShader, sphereMesh, Vec3(0, -101, 0), 100.0, m1);

    uint m2 = raytracerAddMaterial(Vec3(0.1, 0.5, 1.0), 0.01, Vec3(0.1, 0.5, 1.0), 0.1, Vec3(0, 0, 0), 0, 0, 1);
    newSphere(surfaceShader, sphereMesh, Vec3(0.0, 0.5, 3.0), 1.5, m2);

    uint m3 = raytracerAddMaterial(Vec3(0, 0, 0), 0, Vec3(0, 0, 0), 0, Vec3(1, 0, 1), 20, 0, 1);
    newSphere(surfaceShader, sphereMesh, Vec3(0, -.5, -2), 0.5, m3);

    uint m4 = raytracerAddMaterial(Vec3(1, 1, 1), 0, Vec3(1, 1, 1), 0.02, vec3_zero, 0, 1.0, 1.45);
    newSphere(surfaceShader, sphereMesh, Vec3(-4, 0.32, 6), 1.5, m4);

    raytracerSendScene();

    // Create camera
    sc_obj* cameraObj = freeCam_addDefault(vec3_zero, quat_identity, 60.0*DEG_TO_RAD);
    // sc_obj* cameraObj = mouseCam_addDefault(vec3_zero, quat_identity, 60.0*DEG_TO_RAD);
    cam* camera = scobjGetExtData(cameraObj, cam);
    RESetRenderCamera(camera);

    // Create Sun
    sc_obj* sun = newSceneObject(vec3_zero, quat_identity, vec3_one, NULL, false, updateDaylightCycle);
    light* sunLight = scobjAttachLight_Directional(sun, scale3(Vec3(1, 1, 0.9), 70.0));
    scobjAttachDaylightCycle(sun, vec3Unit(0, 0, 1), 120, PI);
    REbackground_skySetSun(sunLight);

    frame_buffer* fb0 = REGetOutputFB(APP->renderEnvironment);
    frame_buffer* fb1 = newFrameBuffer(Uvec2(SGE_BASE_WIDTH, SGE_BASE_HEIGHT));
    FBAttachColor(fb1, tex2DGenFormat(TEX_COL_RGBA, TEX_BIT_DEPTH_16, TEX_FORM_FLOAT));
    if (!isFBComplete(fb1)) failWithError("FB1 not complete", 0);

    bool render = false;
    bool RTX = false;
    printf("# MODE = [SGE]\n");
    while (!appShouldClose()) {
        startFrameUpdate(); // START FRAME
        if (inputIsKeyPressed(SGE_KEY_ESCAPE)) appSetShouldClose(true);
        
        // Switch render modes with TAB
        if (inputIsKeyPressed(SGE_KEY_TAB) && !render) {
            RTX = !RTX;
            printf("# MODE = [%s]\n", RTX ? "RTX" : "SGE");
        }
        // When raytracing, activate ACCUMULATION with ENTER
        if (inputIsKeyPressed(SGE_KEY_ENTER) && RTX) {
            render = !render;
            if (render) {
                printf("[RTX] DOING ACCUMULATION\n");
                raytracerSetNbBounces(15);
                raytracerSetNbTraces(250);
            }
            else {
                printf("[RTX] DOING REALTIME\n");
                raytracerSetNbBounces(6);
                raytracerSetNbTraces(25);
            }
        }
        // When doing accumulation, stop scene updates so the scene stays fixed
        if (!RTX || !render) sceneUpdate(APP->scene);

        if (RTX) {
            if (!render) {
                // If not accumulating, update environment
                REbackground_skyRender(REGetBackgroundData);
                REUpdateGPUEnvironmentBuffer();
                REUpdateGPUCameraBuffer(camera);
                raytracerRestart();
            }
            raytracerRender();
            blitHdrToLdrFB(fbRT, fb1);
            blitToScreenFB(fb1);
        }
        else {
            RERenderSceneFB(fb0);
            blitHdrToLdrFB(fb0, fb1);
            blitToScreenFB(fb1);
        }

        endFrameUpdate(); // END FRAME
    }

    destroyApp();
    return 0;
}