# SUPSY GAME ENGINE
## Introduction
The SupSyGameEngine (SGE) is a custom game engine which aims at making it very easy to start a new project. This project is mainly a learning experiment, but I have already used it in other projects for prototyping with great success.

## /!\ IMPORTANT /!\
The SGE is a work in progress, therefore it has a few quirks and is not as polished as I want it to be. Some functions don't yet have a full description and some importing formats are currently either only partially supported or not supported at all (even though their names appear in the code). In addition, there still isn't a proper system for disposing of the objects allocated during the lifetime of the program when it terminates (meaning it is the user's responsability to deallocate all of the objects he created, which is not ideal).
Furthermore, text rendering is not yet implemented which is very limiting, but I am working on that and am very close to incorporating it into the library (I just have a few rogue pixels to catch).

## Prerequisites
The SGE currently depends on three libraries:
- The [**SupSyLibraries**](https://github.com/SupSyDeMarciou/SupSyLibraries), developped by yours truly.
- [**GLFW3**](https://www.glfw.org/) for creating the window, OpenGL context and for accessing the mouse and keyboard inputs.
- [**GLAD**](https://glad.dav1d.de/#language=c&specification=gl&api=gl%3D4.6&api=gles1%3Dnone&api=gles2%3Dnone&api=glsc2%3Dnone&profile=compatibility&loader=on) with at least the specifications baked into this link.

In addition, after downloading the library, make sure to change the value `SGE_ROOT_PATH` line 10 in file `include/SGE/SGEstructures.h` to the path of the `SGE/..` folder on your machine, as the SGE needs to have access to it when loading built-in shaders (and other built-in files I might want to add later on). Also, if the **SL** is not in the same folder as the **SGE**, make sure to modify the `#include` in line 9 of `include/SGE/SGEstructures.h`. Those, saddly, are some of the quirks mentionned above.

Finaly, the general syntax ruleset used throughout the project is identical to that of the **SL** and is outlined in the README.md of the [**SupSyLibraries GitHub page**](https://github.com/SupSyDeMarciou/SupSyLibraries).

## Content Overview
### Application
An instance of `application` must always be initialized to create the window and OpenGL context needed for rendering. This instance will then persist until the destruction of the program under the global variable `APP` accessible from anywhere in the code. This is the heart of the application from which the rest can sprout.

When the application is created, it in turn creates a **scene** and a **render environment**, both of which are containers for the data necessary for either making the application dynamic or rendering it to the screen. They are part of the default *update and render loops* but are not mandatory when creating an application with the SGE.

### Rendering
The **render environment** created by the **applcation** has its own *methods* under the prefix `RE` and can be accessed from the `APP` instance. It manages the objects which have a *render_obj* or *light* **external data** and the general environment when rendering the scene.

### OpenGL interface
The SGE defines a few types to integrate OpenGL routines into C code as seamlessly as possible:
- `shader`: currently just the OpenGL Object ID with a few wrapper functions, but will be expanded to optimise uniform access and keep track of bindings. It has its own *methods* under the prefix `shader` and has two sub-types:
    - `post_shader`: standing for "post process shader", it is a shader program created with vertex shader `!render/shaders/screen.vs` and is intended to be used with the `blit` and `blitFB` functions. It has its own *methods* under the prefix `postShader`.
    - `comp_shader`: stands for "compute shader", it is a shader program with a compute shader. It has its own *methods* under the prefix `compShader`.
- `texture2D`: structure holding the height, width, format and sampling options of an OpenGL 2D texture. It has its own *methods* under the prefix `tex2D`.
- `frame_buffer`: structure holding the height, width, color texture attachments and depth-stencil attachment of an OpenGL frame buffer. The *color attachments* can be accessed directly as individual `texture2D` objects. It has its own *methods* under the prefix `FB`.
- `mesh`: the representation of a 3D object. Arguably less of a direct OpenGL object translation, it holds a vertex buffer object, element buffer object and vertex array object to allow for easier manipulation of data and render calls. It has its own *methods* under the prefix `mesh`.

As long as the OpenGL context has been created and is maintained, these structures can be used even in isolation from the rest of the SGE, meaning that they are not tied to the default render loop.

### Scene
The **scene** holds the list of all **scene object**s. Its only useful *method* (currently) is `sceneUpdate` which calls the update function on all of the objects eligible for updates.

A **scene object** of type `sc_obj` is a general purpose type which has a transform (position, rotation, scale, parent), may have an update function and **external data** blocks. It has its own *methods* under the prefix `scobj`. Its data blocks are the main interfaces between the *update* and *render* loops and can be created by a user of the librairy too to implement their own behaviours. Some builtin **external data** include:
- `cam`: the camera data needed for 3D rendring. It has its own *methods* under the prefix `cam`.
- `render_obj`: holds the mesh and materials of an objects and registers it as needing to be rendered. It has its own *methods* under the prefix `RO`.
- `light`: holds the data for light information. It has its own *methods* under the prefix `light`.

Each **external data** must be registered as such.

### Input
A few utility functions are grouped under the prefix `input` and deal with *keyboard* or *mouse* interactions. Each key and mouse button has a unique identifier and its state can be queried at the any point using `enum SGE_Key_State inputGetKeyState(enum SGE_Key key)` or simply checked for using `bool inputIsKey(enum SGE_Key key, enum SGE_Key_State state)`, `bool inputIsKeyPressed(enum SGE_Key key);`, `bool inputIsKeyDown(enum SGE_Key key);`, `bool inputIsKeyReleased(enum SGE_Key key);`, `bool inputIsKeyUp(enum SGE_Key key);` or `bool inputIsKeyRepeat(enum SGE_Key key);`. For more flexibility, **key states** can be binary-or-ed to check for multiple states at once and keys `SGE_KEY_ANY` and `SGE_MOUSE_ANY` used for broader reach.

### Debug
Currently a bit light in features. <br>
Functions with prefix `debugDraw` allow for the rendering of primitive shapes with bezier curves for debugging purposes. The thickness and color of the splines can be changes using `void debugDraw_SetLineWidth(float width);` and `void debugDraw_SetColor(vec3 color);`. These parameters then apply for the following splines. <br>
Functions with prefix `debugTimer` can be used for basic timing.

### Text
Enables the import of any TTF font and rendering of text in this font, both in 2D and 3D.

----

## Creating an executable using the SupSyGameEngine
The simplest structure for an SGE executable is as follows:

    #include <SGE.h>

    int main() {

        initializeApp("My First SGE App"); // Create window, OpenGL context, initialize debug features, etc...

        while (!appShouldClose()) { // Wether the window should shut down
            startFrameUpdate(); // Setup the inputs, timers, queries, etc...

            // Your code here...

            endFrameUpdate(); // Calculate TIME.dt, swap buffers, etc...
        }

    }

However, it only shows a black screen, which is not very interesting. <br>
We can incorporate the entire *update* and *render* loops as such:

    // Override the default resolution of the window buffer
    #define SGE_BASE_WIDTH 1920 * 2
    #define SGE_BASE_HEIGHT 1080 * 2
    
    // Notice that the width and height have been defined BEFORE including "SGE.h"
    #include <SGE.h>
    #include <SGE/builtin/extData/mouseCam.h> // Include a camera controled by keyboard and mouse

    int main() {

        initializeApp("My Complete SGE App"); // Create window, OpenGL context, initialize debug features, etc...
        registerMouseCam(); // Register the "mouse_cam" external data

        // Create camera to be used for rendering
        sc_obj* cameraObj = mouseCam_addDefault(Vec3(0, 1, 0), quat_identity, 60);
        cam* cameraData = scobjGetExtData(cameraObj, cam);
        RESetRenderCamera(APP->renderEnvironment, cameraData);

        // Create a basic floor so there is something to look at
        shader surfaceShader = createSurfaceShader("!builtin/shaders/pbr.fs");
        material* planeMat = newMaterial_PBR(surfaceShader, Vec4(1.0, 1.0, 1.0, 1.0), 1.0, 1.0, 1.0, Vec3(0, 0, 0));
        mesh* planeMesh = meshUnitQuad();
        sc_obj* plane = newSceneObject(vec3_zero, Quat_Euler(0.0, -PI/2, 0.0), vec3One(8.0), NULL, true, NULL);
        scobjAttachRenderObject_SingleMat(plane, planeMesh, planeMat, true, RENDER_CULL_FRONT, true, NULL, NULL);

        // Make a sun to have a light in the scene
        quat sunRot = Quat_Euler(70.0, 60.0, 0.0);
        sc_obj* sun = newSceneObject(vec3_zero, sunRot, vec3_one, NULL, false, NULL);
        scobjAttachLight_Directional(sun, scale3(Vec3(1.0, 1.0, 0.95), 15.0));

        // Create an intermediate frame buffer for the HDR to LDR conversion
        frame_buffer* fb = newFrameBuffer(Uvec2(SGE_BASE_WIDTH, SGE_BASE_HEIGHT));
        FBAttachColor(fb, tex2DGenFormat(TEX_COL_RGBA, TEX_BIT_DEPTH_16, TEX_FORM_FLOAT));
        if (!isFBComplete(fb)) failWithError("FB not complete! (OpenGL is very mad...)");

        while (!appShouldClose()) { // Wether the window should shut down
            startFrameUpdate(); // Setup the inputs, timers, queries, etc...

            // Rotate the sun around the X axis each frame to have a daylight cycle
            quat rot = Quat_Euler(0.0, PI / 25.0 * TIME.dt, 0.0);
            mulQ_(&rot, &sunRot, &sun->transform.rotation);

            sceneUpdate(APP->scene);                // Update all scene objects which are active, non-static and have an update function. In this case, only the camera.
            RErenderScene();                        // Render all of the objects with render object external data. In this case, only the plane.
            blitHdrToLdrFB(REGetOutputFB(re), fb);  // Convert High Dynamic Range render into Low Dynamic Range
            blitToScreenFB(fb);                     // Finaly show our final result on screen!

            endFrameUpdate(); // Calculate TIME.dt, swap buffers, etc...
        }
    }

Here we finaly have something to play around with, even if in a limited way. To keep the main loop less cluttered and add better functionality, we might want to abstract away the *sun* daylight cycle into an **external data** like so:

`daylight_cycle.c`:

    #include "daylightCycle.h"
    DEF_EXT_ID_C(daylight_cycle)

    void updateDaylightCycle(sc_obj* object) {
        daylight_cycle* data = scobjGetExtData(object, daylight_cycle); // Retrieve the external data

        data->currentAngle += data->rotationSpeed * TIME.dt; // Increase angle with constant velocity
        setQ_AngleAxis_(&object->transform.rotation, data->currentAngle, &data->rotationAxis); // Apply rotation to the sun
    }

    void scobjAttachDaylightCycle(sc_obj* source, vec3 rotationAxis, double revolutionTime, double initialAngle) {
        daylight_cycle* new = (daylight_cycle*)malloc(sizeof(daylight_cycle));
        
        new->rotationAxis = rotationAxis;
        new->rotationSpeed = TAU / revolutionTime;
        new->currentAngle = initialAngle;

        scobjAddExtData(source, daylight_cycle, new); // Add the external data to the source
    }

    sc_obj* daylightCycle_addDefault(vec3 rotationAxis, double revolutionTime, double initialAngle) {
        sc_obj* sunTransform = newSceneObject(vec3_zero, quat_identity, vec3_one, NULL, false, &updateDaylightCycle);
        scobjAttachDaylightCycle(sunTransform, rotationAxis, revolutionTime, initialAngle);
        return sunTransform;
    }

    void registerDaylightCycle() {
        extDataRegister(&EXT_ID(daylight_cycle), &free);
    }

`daylight_cycle.h`:

    #ifndef __SGE_BUILTIN_DAYLIGHT_CYCLE_H__
    #define __SGE_BUILTIN_DAYLIGHT_CYCLE_H__

    #include "../../../SGE.h"

    /// @brief Daylight cycle external data structure
    typedef struct DaylightCycle {
        vec3 rotationAxis;
        double currentAngle;
        double rotationSpeed;
    } daylight_cycle;
    DEF_EXT_ID(daylight_cycle)

    /// @brief Add a new free cam external data block
    /// @param source The scene object to which this data is to be added
    /// @param rotationAxis The axis arround which to rotate the sun
    /// @param revolutionTime The time it takes to make a full revolution in seconds
    /// @param initialAngle The initial angle of the sun in the sky
    void scobjAttachDaylightCycle(sc_obj* source, vec3 rotationAxis, double revolutionTime, double initialAngle);

    /// @brief The update function associated with 'FreeCamExternalData'
    /// @param object The object on which to call the update function
    void updateDaylightCycle(sc_obj* object);

    /// @brief Add the default daylightCycle sun to the scene
    /// @param revolutionTime The time in seconds for a full daylight cycle to occur
    /// @param rotationAxis The axis around which revolves the sun
    /// @param initialAngle The initial angle of the sun in the sky
    /// @return The newly created and added daylightCycle dummy object
    sc_obj* daylightCycle_addDefault(vec3 rotationAxis, double revolutionTime, double initialAngle);

    /// @brief Register the daylight cycle external data block
    void registerDaylightCycle();

    #endif

And then remove the rotation inside the main loop and juste add before it:
    
    scobjAttachDaylightCycle(sun, vec3Unit(1, 0, 0), 120.0, 0.0);

Though it takes more effort, we now have a more versatile *sun* **external data** which we can use in multiple projects by just sliding it into a `SGE/custom` folder with a nice `.h` header file and lovely comments.
