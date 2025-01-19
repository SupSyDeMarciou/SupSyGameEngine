#ifndef __SGE_H__
#define __SGE_H__

#ifdef SGE_MAIN
#   include <glad/glad.c>
    char* SGE_ROOT_PATH = "C:/msys64/mingw64/include/SupSy/SGE"; // Change to the path to the SGE on your machine
    _Bool SGE_DO_WARNING = 1;                                    // Set to "false" to disable warning messages
#endif

#include "SGE/application/application.h"
#include "SGE/application/input.h"

#include "SGE/debug/debug.h"

#include "SGE/render/render.h"
#include "SGE/render/material.h"
#include "SGE/render/mesh.h"
#include "SGE/render/rend_env.h"
#include "SGE/render/light.h"
#include "SGE/render/text.h"
#include "SGE/render/shaders/postProcessShader.h"
#include "SGE/render/shaders/shader.h"
#include "SGE/render/textures/frameBuffer.h"
#include "SGE/render/textures/texture.h"

#include "SGE/scene/scene.h"
#include "SGE/scene/sc_object.h"

#include "SGE/constants.h"

#endif