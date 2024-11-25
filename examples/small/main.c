// Override the default resolution of the window buffer
#define SGE_BASE_HEIGHT 2160
#define SGE_BASE_WIDTH 3840

// Notice that the width and height have been defined BEFORE including "SGE.h"
#include <SupSy/SGE.h>
#include <SupSy/SGE/builtin/extData/mouseCam.h> // Include a camera controled by keyboard and mouse

int main() {

    initializeApp("My Complete SGE App"); // Create window, OpenGL context, initialize debug features, etc...
    registerMouseCam(); // Register the "mouse_cam" external data

    // Create camera to be used for rendering
    sc_obj* cameraObj = mouseCam_addDefault(Vec3(0, 1, 0), quat_identity, 60*DEG_TO_RAD);
    cam* cameraData = scobjGetExtData(cameraObj, cam);
    RESetRenderCamera(cameraData);
    // Make mouse cursor hidden for infinite panning
    glfwSetInputMode(APP->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Create a basic floor so there is something to look at
    shader surfaceShader = createSurfaceShader("!builtin/shaders/pbr.fs");
    material* planeMat = newMaterial_PBR(surfaceShader, Vec4(1.0, 1.0, 1.0, 1.0), 1.0, 1.0, 1.0, Vec3(0, 0, 0));
    mesh* planeMesh = meshUnitQuad();
    sc_obj* plane = newSceneObject(vec3_zero, Quat_Euler(0.0, -PI/2, 0.0), vec3One(8.0), NULL, true, NULL);
    scobjAttachRenderObject_SingleMat(plane, planeMesh, planeMat, true, RENDER_CULL_FRONT, true, NULL, NULL);

    // Make a sun to have a light in the scene
    quat sunRot = Quat_Euler(70.0, 60.0, 0.0);
    sc_obj* sun = newSceneObject(vec3_zero, sunRot, vec3_one, NULL, false, NULL);
    light* sunLight = scobjAttachLight_Directional(sun, scale3(Vec3(1.0, 1.0, 0.95), 15.0));
    simpleSkySetSun(REGetBackground(), sunLight);

    // Get reference to the output of the SGE Rendering Pipeline
    frame_buffer* REoutput = REGetOutputFB(APP->renderEnvironment);
    // Create an intermediate frame buffer for the HDR to LDR conversion
    frame_buffer* fb = newFrameBuffer(Uvec2(SGE_BASE_WIDTH, SGE_BASE_HEIGHT));
    FBAttachColor(fb, tex2DGenFormat(TEX_COL_RGBA, TEX_BIT_DEPTH_16, TEX_FORM_FLOAT));
    if (!isFBComplete(fb)) failWithError("FB not complete! (OpenGL is very mad...)");

    while (!appShouldClose()) { // Wether the window should shut down
        startFrameUpdate(); // Setup the inputs, timers, queries, etc...
        // As the mouse is hidden, we need to be able to close the window without pressing the "X"
        if (inputIsKeyPressed(SGE_KEY_ESCAPE)) appSetShouldClose(true);

        // Rotate the sun around the X axis each frame to have a daylight cycle
        quat rot = Quat_Euler(0.0, PI / 25.0 * TIME.timeAtFrameStart, 0.0);
        mulQ_(&rot, &sunRot, &sun->transform.rotation);

        sceneUpdate(APP->scene);                // Update all scene objects which are active, non-static and have an update function. In this case, only the camera.
        RERenderScene();                        // Render all of the objects with render object external data. In this case, only the plane.
        blitHdrToLdrFB(REoutput, fb);           // Convert High Dynamic Range render into Low Dynamic Range
        blitToScreenFB(fb);                     // Finaly show our final result on screen!

        endFrameUpdate(); // Calculate TIME.dt, swap buffers, etc...
    }

    destroyApp();
    return 0;
}