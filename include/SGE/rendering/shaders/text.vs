#version 430 core

#include "!render/shaders/text.glsl"

// Vertex
layout (location = 0) in vec3 aPos;
layout (location = 3) in vec2 aUv;

layout (location = 4) in vec2 aTextOffset;
layout (location = 5) in uint aTextIdx;
layout (location = 6) in vec4 aTextCol;
layout (location = 7) in float aTextSize;

out vec2 TexCoords;
flat out uint GlyphIdx;
flat out vec4 GlyphCol;

void main()
{
    TexCoords = aUv;
    GlyphIdx = aTextIdx;
    GlyphCol = aTextCol;
    ivec2 offset = b_GlyphData[GlyphIdx].coords.xy;
    ivec2 scale = b_GlyphData[GlyphIdx].coords.zw;
    gl_Position = vec4((((aPos.xy * scale + offset) * aTextSize + aTextOffset) / float(u_EM) + u_Position) * vec2(1.0, u_Ratio), 0, 1);
}