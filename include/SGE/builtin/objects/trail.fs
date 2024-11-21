#version 330 core

uniform vec3 u_ColorStart;
uniform vec3 u_ColorEnd;

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
    vec3 color = mix(u_ColorEnd, u_ColorStart, fs_in.uv.x * fs_in.uv.x);
    FragColor = vec4(color, 1.0);
    FragNormal = vec4(fs_in.normal * 0.5 + 0.5, 1.0);
}