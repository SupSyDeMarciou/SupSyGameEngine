#version 330 core

uniform vec4 mColor = vec4(1);

uniform sampler2D mAlbedoMap;
uniform bool mAlbedoMapSet = false;
uniform sampler2D mNormalMap;
uniform bool mNormalMapSet = false;

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
    vec3 color = mAlbedoMapSet ? texture2D(mAlbedoMap, fs_in.uv).rgb * mColor.rgb : mColor.rgb;
    vec3 normal = mAlbedoMapSet ? texture2D(mNormalMap, fs_in.uv).xyz * 2.0 - 1.0 : fs_in.normal; // mNormalnMap

    FragColor = vec4(color, 1.0);
    FragNormal = vec4(normal * 0.5 + 0.5, 1.0);
}