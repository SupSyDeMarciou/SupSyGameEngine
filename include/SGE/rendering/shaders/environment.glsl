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