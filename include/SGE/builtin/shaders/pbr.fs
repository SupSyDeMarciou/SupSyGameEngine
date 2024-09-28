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
float DistributionGGX(float dotNH, float rougness) {
    float a2 = rougness * rougness;
	
    float t = (dotNH * dotNH * (a2 - 1.0) + 1.0);
	
    return a2 / (PI * t * t);
}

float GeometrySchlickGGX(float dotNO, float k) {
    return dotNO / (dotNO * (1.0 - k) + k);
}
  
float GeometrySmith(vec3 normal, vec3 view, vec3 light, float roughness) {
    float k = (roughness + 1) * (roughness + 1) * 0.125; // direct remapping
    float ggx1 = GeometrySchlickGGX(max(dot(normal, view), 0.0), k);
    float ggx2 = GeometrySchlickGGX(max(dot(normal, light), 0.0), k);
	
    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float dotNH, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - dotNH, 0, 1), 5.0);
}

void main()
{
    vec3 pos = fs_in.position.xyz;
    vec3 viewDir = normalize(u_CamPosition - pos);
    vec3 Lo = vec3(0);

    vec2 uv = fs_in.uv;
    vec4 albedo = (mAlbedoMapSet ? texture2D(mAlbedoMap, uv) * mColor : mColor) * vec4(fs_in.color, 1);
    float roughness = mRoughness;
    float metalic = mMetalic;
    float specular = mSpecular;
    if (mRMSMapSet) {
        vec3 RMS = texture2D(mRMSMap, uv).rgb;
        metalic *= RMS.x;
        roughness *= RMS.y;
        specular *= RMS.z;
    }
    vec3 emission = mEmissionMapSet ? texture2D(mEmissionMap, uv).rgb + mEmission : mEmission;
    vec3 normal = fs_in.normal;
    if (mNormalMapSet) {
        vec3 normalTex = texture2D(mNormalMap, uv).xyz * 2.0 - 1.0;
        normal = normalize(u_ObjTransformNormalMat * vec3(normalTex.x, normalTex.z, -normalTex.y));
    }

    // LOOP OVER LIGHTS
    for (uint i = 0u; i < u_EnvNbLights; i++) {
        vec3 lightCol = u_EnvLightCol[i];
        vec3 lightDir = u_EnvLightType[i] == LIGHT_POINT ? normalize(u_EnvLightPos[i] - pos) : -u_EnvLightDir[i];
        vec3 halfwayDir = normalize(lightDir + viewDir);

        vec3 F0 = vec3(0.04); // Base response to normal incidence for non-metalic materials
        F0 = mix(F0, albedo.rgb, metalic); // Metalic materials don't diffuse so mColor.rgb will be their response to normal incidence

        float D = DistributionGGX(max(dot(normal, halfwayDir), 0), roughness);
        vec3 F = FresnelSchlick(max(dot(viewDir, halfwayDir), 0), F0);
        float G = GeometrySmith(normal, viewDir, lightDir, roughness);

        vec3 specularCol = D*F*G / (4 * max(dot(normal, viewDir), 0) * max(dot(normal, lightDir), 0) + 0.001); // To avoid division by 0
        vec3 diffuseCol = (1.0 - metalic) * (vec3(1) - F);

        float dist = length(u_EnvLightPos[i] - pos);
        float attenuation = u_EnvLightType[i] == LIGHT_DIR ? 1.0 : 1.0 / (dist * dist); // Inverse square law for non-directrional light sources
        vec3 radianceCol = lightCol * attenuation;

        Lo += (diffuseCol * albedo.rgb / PI + specularCol * specular) * max(dot(normal, lightDir), 0) * radianceCol;
    }

    vec3 ambiant = u_EnvAmbiant * albedo.rgb * (dot(normal, vec3(0, 1, 0)) * 0.4 + 0.6);
    vec3 color = Lo + ambiant + emission;
    FragColor = vec4(color, 1.0);
    FragNormal = vec4(normal * 0.5 + 0.5, 1.0);
}