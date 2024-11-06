#version 330 core

#include "!rendering/shaders/camera"
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

// Vertex
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aCol;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aUv;

// Vertex shader output
out VS_OUT {
    vec4 position;
    vec3 color;
    vec3 normal;
    vec2 uv;
} vs_out;

void main()
{
    s_out.position = u_ObjTransform * vec4(aPos, 1);
    vs_out.color = aCol;
    vs_out.normal = normalize(u_ObjTransformNormal * aNormal);
    vs_out.uv = aUv;

    vec3 t = u_CamInvRotation * (vs_out.position - u_CamPosition);
    gl_Position = vec4(t.x * u_CamData.w / u_CamData.z, t.y, t.z / (u_CamData.y - u_CamData.x), 1);
}