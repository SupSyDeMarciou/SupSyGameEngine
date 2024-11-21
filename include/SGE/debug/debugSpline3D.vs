#version 330

#include "!render/shaders/camera.glsl"

// Vertex
layout (location = 0) in vec3 aPos;
layout (location = 4) in vec3 aSplineP0;
layout (location = 5) in vec3 aSplineP1;
layout (location = 6) in vec3 aSplineP2;
layout (location = 7) in vec3 aColor;
layout (location = 8) in float aThickness;

out VS_OUT {
    vec2 position;
    vec2 p0;
    vec2 p1;
    vec2 p2;

    vec3 color;
    float thickness;
} vs_out;

void main() {

    vec4 p0 = u_CamRenderMat * vec4(aSplineP0, 1);
    vec4 p1 = u_CamRenderMat * vec4(aSplineP1, 1);
    vec4 p2 = u_CamRenderMat * vec4(aSplineP2, 1);

    vs_out.p0 = p0.xy / p0.w;
    vs_out.p1 = p1.xy / p1.w;
    vs_out.p2 = p2.xy / p2.w;

    vec2 maxP = max(max(vs_out.p0, vs_out.p1), vs_out.p2);  
    vec2 minP = min(min(vs_out.p0, vs_out.p1), vs_out.p2);
    vec2 dim = (maxP - minP) + 2.0 * aThickness;
    vec2 pos = (maxP + minP) * 0.5;

    gl_Position = vec4(aPos.xy * dim + pos, 0.0, 1.0);
    vs_out.position = gl_Position.xy;

    vs_out.color = aColor;
    vs_out.thickness = p0.w <= 0.0 || p1.w <= 0.0 || p2.w <= 0.0 ? -1.0 : aThickness;
}