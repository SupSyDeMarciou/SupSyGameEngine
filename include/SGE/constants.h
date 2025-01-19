#ifndef __SGE_CONSTANTS_H__
#define __SGE_CONSTANTS_H__

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

extern bool  SGE_DO_WARNING;
extern char* SGE_ROOT_PATH;

#include "../SL.h" // Change to the path to the SL on your machine

#ifndef SGE_BASE_WIDTH
#   define SGE_BASE_WIDTH 1920
#endif
#ifndef SGE_BASE_HEIGHT
#   define SGE_BASE_HEIGHT 1080
#endif

#define SGE_REND_ENV_MAX_OBJECTS_TO_RENDER 8192
#define SGE_REND_ENV_MAX_LIGHTS_TO_RENDER 128

/// @brief Function which frees an object
/// @param toFree The object to free
typedef void func_free(void* toFree);
/// @brief Function which destroys an object
/// @param toDestroy The object to destroy
typedef void func_destroy(void* toDestroy);

void destroyApp();

#define SGE_NO_WARNING(code) do { bool __SGE_LAST_WARNING_STATE__ = SGE_DO_WARNING; SGE_DO_WARNING = false; code; SGE_DO_WARNING = __SGE_LAST_WARNING_STATE__; } while (false)
#define SGE_throwWarning(message, ...) do { if (SGE_DO_WARNING) fprintf(stderr, TG_COLOR(TGC_YELLOW,TGC_FG)"[SGE] WARNING - [%s]:\n\t"message TG_COLOR(TGC_YELLOW,TGC_FG)"\nat %s:%d"T_GRAPHICS(TG_RESET)"\n", __FUNCTION__, ##__VA_ARGS__, __FILE__, __LINE__); } while (false)
#define SGE_throwError(message, ...) do { fprintf(stderr, TG_COLOR(TGC_RED,TGC_FG)"[SGE] ERROR - [%s]:\n\t"message TG_COLOR(TGC_RED,TGC_FG)"\nat %s:%d"T_GRAPHICS(TG_RESET)"\n", __FUNCTION__, ##__VA_ARGS__, __FILE__, __LINE__); } while (false)
#define SGE_fail(message, ...) do { fprintf(stderr, TG_COLOR(TGC_RED,TGC_FG)"[SGE] FATAL ERROR - [%s]:\n\t"message TG_COLOR(TGC_RED,TGC_FG)"\nat %s:%d"T_GRAPHICS(TG_RESET)"\n", __FUNCTION__, ##__VA_ARGS__, __FILE__, __LINE__); destroyApp(); exit(EXIT_FAILURE); } while (false)
#define SGE_ifail(message, ...) do { fprintf(stderr, TG_COLOR(TGC_MAGENTA,TGC_FG)"[SGE] UNIMPLEMENTED - [%s]:\n\t"message TG_COLOR(TGC_MAGENTA,TGC_FG)"\nat %s:%d"T_GRAPHICS(TG_RESET)"\n", __FUNCTION__, ##__VA_ARGS__, __FILE__, __LINE__); destroyApp(); exit(EXIT_FAILURE); } while (false)

#endif