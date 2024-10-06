#version 330 core

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
    vs_out.position = u_ObjTransformMat * vec4(aPos, 1);
    vs_out.color = aCol;
    vs_out.normal = normalize(u_ObjTransformNormalMat * aNormal);
    vs_out.uv = aUv;

    gl_Position = u_CamRenderMat * vs_out.position;
}