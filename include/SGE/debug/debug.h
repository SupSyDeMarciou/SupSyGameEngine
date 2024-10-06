#ifndef __SGE_DEBUG_DEBUG
#define __SGE_DEBUG_DEBUG

#include "../../SGE.h"
#include "../rendering/textures/frameBuffer.h"

/// @brief Initialize debugging tools
void debugInitialize();

/// @brief Set line width for next debug renders
/// @param width The new width
void debugSetLineWidth(float width);
/// @brief Set color for next debug renders
/// @param width The new color
void debugSetColor(vec3 color);
/// @brief Set wether points should render
/// @param value If points should render
void debugSetRenderDots(bool value);

/// @brief Render all scheduled splines
/// @param fb The frame buffer on which to render
void debugDoRender(frame_buffer* fb);

/// @brief Render a spline
/// @param p0 Starting point
/// @param p1 Control point
/// @param p2 End point
void debugRenderSpline(vec3 p0, vec3 p1, vec3 p2);

/// @brief Render a line segment
/// @param p0 Starting point
/// @param p2 End point
void debugRenderSegment(vec3 p0, vec3 p2);
/// @brief Render a 3D box
/// @param center The center of the box
/// @param size The size of the box
void debugRenderBox(vec3 center, vec3 size);
/// @brief Render a 2D circle
/// @param center The center of the circle
/// @param normal The normal of the circle
/// @param radius The radius of the circle
void debugRenderCircle(vec3 center, vec3 normal, float radius);

#endif