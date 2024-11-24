#ifndef __SGE_CONSTANTS_H__
#define __SGE_CONSTANTS_H__

#include <stdlib.h>
#include <stdio.h>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "../SL.h" // Change to the path to the SL on your machine
extern char* SGE_ROOT_PATH; // Change to path to the SGE on your machine

#ifndef SGE_BASE_WIDTH
#   define SGE_BASE_WIDTH 1920
#endif
#ifndef SGE_BASE_HEIGHT
#   define SGE_BASE_HEIGHT 1080
#endif

#define SGE_REND_ENV_MAX_OBJECTS_TO_RENDER 8192
#define SGE_REND_ENV_MAX_LIGHTS_TO_RENDER 128

#endif