#ifndef __SGE_RENDER_MATERIAL_H__
#define __SGE_RENDER_MATERIAL_H__

#include "../SGEconstants.h"

#include "shaders/shaders.h"
#include "textures/texture.h"

typedef struct Material material;

typedef void func_shaderSendMatData(material* materialData);
typedef void func_freeMaterialData(void* materialData);

struct Material {
    shader s;
    func_shaderSendMatData* sendDataFunction;
    func_freeMaterialData* freeMaterialData;
    void* materialData;
};

material* newMaterial(shader s, func_shaderSendMatData* sendDataFunction, void* materialData, func_freeMaterialData* freeMaterialData);

void freeMaterial(material* toFree);

void shaderSetMaterialData(material* mat);



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////// * * *  PRE-MADE MATERIALS  * * * /////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



typedef struct StandardMaterialData {
    vec4 color;
    float roughness;
    float metalic;
    float specular;
    vec3 emission;
} std_mat_data;

material* newMaterial_PBR(shader s, vec4 color, float roughness, float metalic, float specular, vec3 emission);


typedef struct StandardTexturedMaterialData {
    vec4 color;
    float roughness;
    float metalic;
    float specular;
    vec3 emission;

    texture2D* albedo;
    texture2D* normalMap;
    texture2D* RMSMap;
    texture2D* emissionMap;
} std_tex_mat_data;

material* newMaterial_PBRTextured(shader s, 
    vec4 color, float roughness, float metalic, float specular, vec3 emission,
    texture2D* albedoMap, texture2D* normalMap, texture2D* roughnessMetalicSpecularMap,
    texture2D* emissionMap
);

#endif