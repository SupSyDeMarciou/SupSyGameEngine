#ifndef __SGE_APP_APP
#define __SGE_APP_APP

#include <stdarg.h>

#include "../SGEstructures.h"
#include "../scene/scene.h"
#include "../render/render.h"

struct Application
{
    GLFWwindow* window;
    render_env* renderEnvironment;
    scene scene;

    char title[256];
    char fpsString[512];

    bool shouldClose;
};
extern application* APP;

extern struct Time {
    double dt;
    double timeAtFrameStart;

    double fps;
    double smoothFps;

    uint64 frameCount;
} TIME;

/// @brief Initialize the application
/// @param title The title of the application
void initializeApp(const char* title);
/// @brief Initialize the application
/// @param title The title of the application
/// @param createRE Wether or not to create render environment. Frees up some space on CPU and GPU when creating an application which does not use the SupSyGameEngine 3D rendering pipeline 
void initializeApp_RE(const char* title, bool createRE);
/// @brief Destroy the application
void destroyApp();

/// @brief Set the window fullscreen or not
/// @param fullscreen If the window should be fullscreen
void appWindowSetFullscreen(bool fullscreen);
/// @brief Set the mouse and keyboard focus on the app window
void appWindowSetFocus();
/// @brief Check if the application should close
/// @return If the application should close
bool appShouldClose();


///////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// UPDATE/RENDER LOOP ////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////


/// @brief Start a new frame update
/// @note Should be called first in the main loop
void startFrameUpdate();
/// @brief End the current frame update
/// @note Should be called after "startFrameUpdate" inside main loop. Better be called last in loop
void endFrameUpdate();

#endif
