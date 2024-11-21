#version 330 core

#include "!render/shaders/environment.glsl"
#include "!render/shaders/camera.glsl"
#include "!render/shaders/object.glsl"

// Vertex shader output
in VS_OUT {
    vec4 position;
    vec3 color;
    vec3 normal;
    vec2 uv;
} fs_in;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 FragNormal;

void main()
{
    vec3 color = vec3(fs_in.uv, 0.0);
    FragColor = vec4(color, 1.0);
    FragNormal = vec4(fs_in.normal * 0.5 + 0.5, 1.0);
}