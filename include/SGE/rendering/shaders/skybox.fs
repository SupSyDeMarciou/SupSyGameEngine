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
};
uniform uint u_SunIdx = 0u;

// Camera
layout (std140) uniform Camera {
    vec4 u_CamData; // NP, FP, W, H
    vec3 u_CamPosition;
    mat3 u_CamInvRotMat;
    mat4 u_CamProjMat;
    mat4 u_CamRenderMat;
};

// Vertex Shader Output
in vec2 TexCoords;

layout (location = 0) out vec4 o_FragColor;
layout (location = 1) out vec4 o_FragNormal;

void main()
{
    vec3 rayDir = vec3(
        (TexCoords.x * 2.0 - 1.0) * u_CamData.z,
        (TexCoords.y * 2.0 - 1.0) * u_CamData.w,
        u_CamData.x
    );
    rayDir = transpose(u_CamInvRotMat) * normalize(rayDir);

    vec3 sky = (dot(rayDir, vec3(0, 1, 0)) * 0.5 + 0.5) * u_EnvAmbiant;
    vec3 color = sky;

    if (u_EnvNbLights > 0u && u_SunIdx < 128u) {
        float SunRadius = 0.002;
        float sun = max(0, -dot(rayDir, u_EnvLightDir[u_SunIdx]));
        sun = sun > 1 - SunRadius ? 1.0 : 0.0;

        color += sun * u_EnvLightCol[u_SunIdx];
    }

    o_FragColor = vec4(color, 1.0);
    o_FragNormal = vec4(rayDir, 1.0);
}