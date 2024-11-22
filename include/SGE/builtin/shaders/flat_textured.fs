#version 330 core

uniform vec4 mColor = vec4(1);
uniform float mRoughness = 1.0;
uniform float mMetalic = 0.0;
uniform float mSpecular = 1.0;
uniform vec3 mEmission = vec3(0);

uniform sampler2D mAlbedoMap;
uniform bool mAlbedoMapSet = false;
uniform sampler2D mNormalMap;
uniform bool mNormalMapSet = false;
uniform sampler2D mRMSMap;
uniform bool mRMSMapSet = false;
uniform sampler2D mEmissionMap;
uniform bool mEmissionMapSet = false;

// Vertex shader output
in VS_OUT {
    vec4 position;
    vec3 color;
    vec3 normal;
    vec2 uv;
} fs_in;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 FragNormal;

#define PI 3.141592

void main()
{
    vec3 color = mAlbedoMapSet ? texture2D(mAlbedoMap, fs_in.uv).rgb * mColor.rgb : mColor.rgb;
    // vec3 color = vec3(fs_in.uv, 0);
    vec3 normal = fs_in.normal; // mNormalnMap

    FragColor = vec4(color, 1.0);
    FragNormal = vec4(normal * 0.5 + 0.5, 1.0);
}