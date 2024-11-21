#version 330 core

#include "!render/shaders/camera.glsl"
#include "!render/shaders/object.glsl"

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