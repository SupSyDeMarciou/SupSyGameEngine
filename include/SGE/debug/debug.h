#ifndef __SGE_DEBUG_DEBUG
#define __SGE_DEBUG_DEBUG

#include "../constants.h"
#include "../render/textures/frameBuffer.h"

#pragma region DRAWING

/// @brief Set line width for next debug renders
/// @param width The new width
void debugDraw_SetLineWidth(float width);
/// @brief Set color for next debug renders
/// @param width The new color
void debugDraw_SetColor(vec3 color);
/// @brief Set wether points should render
/// @param value If points should render
void debugDraw_SetRenderDots(bool value);

/// @brief Render all scheduled splines
/// @param fb The frame buffer on which to render
void debugDraw_DoRender(frame_buffer* fb);

/// @brief Render a spline in 2D
/// @param p0 Starting point
/// @param p1 Control point
/// @param p2 End point
void debugDraw_Spline2D(vec2 p0, vec2 p1, vec2 p2);
/// @brief Render a spline in 3D
/// @param p0 Starting point
/// @param p1 Control point
/// @param p2 End point
void debugDraw_Spline3D(vec3 p0, vec3 p1, vec3 p2);

/// @brief Render a line segment in 2D
/// @param p0 Starting point
/// @param p2 End point
void debugDraw_Segment2D(vec2 p0, vec2 p2);
/// @brief Render a line segment in 3D
/// @param p0 Starting point
/// @param p2 End point
void debugDraw_Segment3D(vec3 p0, vec3 p2);

/// @brief Render a box in 2D
/// @param center The center of the box
/// @param size The size of the box
/// @param rotation The rotation of the box
void debugDraw_Box2D(vec2 center, vec2 size, float rotation);
/// @brief Render a box in 3D
/// @param center The center of the box
/// @param size The size of the box
/// @param rotation The rotation of the box
void debugDraw_Box3D(vec3 center, vec3 size, quat rotation);

/// @brief Render a circle in 2D
/// @param center The center of the circle
/// @param radius The radius of the circle
void debugDraw_Circle2D(vec2 center, float radius);
/// @brief Render a circle in 3D
/// @param center The center of the circle
/// @param normal The normal of the circle
/// @param radius The radius of the circle
void debugDraw_Circle3D(vec3 center, vec3 normal, float radius);

#pragma endregion DRAWING

#pragma region TIMING

/// @brief Unique identifier for a timer
typedef uint16 timer;

/// @brief Initialize a new timer
/// @param start If the timer is active right away
/// @return The identifier of the newly created timer
timer debugTimer_Init(bool start);
/// @brief Read the elapsed time for this timer
/// @param ID The identifier of the timer
/// @return The total active time of this timer
double debugTimer_Read(timer ID);
/// @brief Activate the timer
/// @param ID The identifier of the timer
void debugTimer_Start(timer ID);
/// @brief Reset the timer to 0.0
/// @param ID The identifier of the timer
void debugTimer_Restart(timer ID);
/// @brief Deactivate the timer
/// @param ID The identifier of the timer
void debugTimer_Stop(timer ID);
/// @brief Destroy the timer, freeing its space for other timers to be used
/// @param ID The identifier of the timer
/// @return The total active time of this timer
double debugTimer_Destroy(timer ID);
/// @brief Print the time the timer was active for
/// @param ID The identifier of the timer
void debugTimer_Print(timer ID);

#pragma region TIMING

/// @brief Print an OpenGL error message if one is present
void debugCheckGLError();

#endif