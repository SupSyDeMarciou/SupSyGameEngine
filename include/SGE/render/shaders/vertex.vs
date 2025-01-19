#version 330 core

#include "!render/shaders/camera.glsl"
#include "!render/shaders/object.glsl"

// Vertex
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aCol;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aUv;

// Vertex shader output
out VS_OUT {
    vec4 position;
    vec3 color;
    vec3 normal;
    vec2 uv;
} vs_out;

out vec4 glPos;

void main()
{
    vs_out.position = u_ObjTransformMat * vec4(aPos, 1);
    vs_out.color = aCol;
    vs_out.normal = normalize(u_ObjTransformNormalMat * aNormal);
    vs_out.uv = aUv;

    glPos = gl_Position = u_CamRenderMat * vs_out.position;
}