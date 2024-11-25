#version 330 core

// Vertex
layout (location = 0) in vec3 aPos;
layout (location = 3) in vec2 aUv;

out vec2 TexCoords;

uniform vec2 u_Position;
uniform vec2 u_Scale;
uniform float u_Ratio;

void main()
{
    TexCoords = aUv;
    gl_Position = vec4((aPos.xy * u_Scale + u_Position) * vec2(1.0, u_Ratio), 0.0, 1.0);
}