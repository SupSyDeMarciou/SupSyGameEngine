#version 330 core

#include "!render/shaders/camera.glsl"

uniform sampler2D Source;
uniform sampler2D u_Depth;

in vec2 TexCoords;

out vec4 FragColor;

void main()
{
    float depth01 = texture2D(u_Depth, TexCoords).r;
    vec3 color = vec3(1.0/10) * linearizeDepth(depth01);
    FragColor = vec4(color, 1);
}