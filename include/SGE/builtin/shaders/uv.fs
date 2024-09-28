#version 330 core

// Environment
#define MAX_NB_LIGHTS 128u
#define LIGHT_NONE 0u
#define LIGHT_DIR 1u
#define LIGHT_POINT 2u
#define LIGHT_SPOT 3u
#define LIGHT_AREA 4u
layout (std140) uniform Environment {
    vec3 u_EnvAmbiant;
    vec3 u_EnvLightDir[MAX_NB_LIGHTS];
    vec3 u_EnvLightCol[MAX_NB_LIGHTS];
    vec3 u_EnvLightPos[MAX_NB_LIGHTS];
    uint u_EnvLightType[MAX_NB_LIGHTS];
    uint u_EnvNbLights;
    vec2 pad;
};

// Camera
layout (std140) uniform Camera {
    vec4 u_CamData; // NP, FP, W, H
    vec3 u_CamPosition;
    mat3 u_CamInvRotMat;
    mat4 u_CamProjMat;
    mat4 u_CamRenderMat;
};

// Object
layout (std140) uniform Object {
    vec3 u_ObjPosition;
    mat3 u_ObjRotMat;
    vec3 u_ObjScale;
    mat3 u_ObjTransformNormalMat;
    mat4 u_ObjTransformMat;
};

// Material
// uniform struct {
//     vec4 color;
//     float roughness;
//     float metalic;
//     float specular;
//     vec3 emission;
// } material;
uniform vec4 mColor;
uniform float mRoughness;
uniform float mMetalic;
uniform float mSpecular;
uniform vec3 mEmission;

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