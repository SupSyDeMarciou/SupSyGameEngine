#version 330 core

// Vertex
layout (location = 0) in vec3 aPos;
layout (location = 3) in vec2 aUv;

out vec2 TexCoords;

void main()
{
    TexCoords = aUv;
    gl_Position = vec4(aPos, 1.0);
}