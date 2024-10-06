#ifndef __SGE_STRUCTURES
#define __SGE_STRUCTURES

#include <stdlib.h>
#include <stdio.h>
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "../SL.h" // Change to the path to the SL on your machine

#ifndef SGE_BASE_WIDTH
    #define SGE_BASE_WIDTH 1920
#endif
#ifndef SGE_BASE_HEIGHT
    #define SGE_BASE_HEIGHT 1080
#endif

typedef struct Application application;
typedef struct Scene scene;
typedef struct RenderEnvironment render_env;

#define __SUPSY_SGE_PATH "C:/msys64/mingw64/include/SupSy/SGE"

#endif