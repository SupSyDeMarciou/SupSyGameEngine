#version 430 core

#include "!render/shaders/text.glsl"

in vec2 TexCoords;
flat in uint GlyphIdx;
flat in vec4 GlyphCol;

out vec4 FragColor;

void main()
{
    if (!insideText(TexCoords, GlyphIdx)) discard;
    
    FragColor = GlyphCol;
}