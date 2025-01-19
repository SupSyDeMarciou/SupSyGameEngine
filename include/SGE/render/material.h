#ifndef __SGE_RENDER_MATERIAL_H__
#define __SGE_RENDER_MATERIAL_H__

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
/// @brief Create a material ON STACK
/// @param s The shader to use
/// @param sendDataFunction The function to call for sending the necessary data to the shader before rendering
/// @param materialData The data stored in this material
/// @param freeMaterialData The function to free the data constained in this material
/// @return The newly create material
material createMaterial(shader s, func_shaderSendMatData* sendDataFunction, void* materialData, func_free* freeMaterialData);
/// @brief Destroy a material
/// @param toDestroy The material to destroy
void destroyMaterial(material toDestroy);
/// @brief Create a new material
/// @param s The shader to use
/// @param sendDataFunction The function to call for sending the necessary data to the shader before rendering
/// @param materialData The data stored in this material
/// @param freeMaterialData The function to free the data constained in this material
/// @return The newly create material
material* newMaterial(shader s, func_shaderSendMatData* sendDataFunction, void* materialData, func_free* freeMaterialData);
/// @brief Free a material
/// @param toDestroy The material to free
void freeMaterial(material* toFree);

/// @brief Use a material for a render
/// @param mat The material to use
void materialUse(material* mat);



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////// * * *  PRE-MADE MATERIALS  * * * /////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



typedef struct PBRMaterialData {
    vec4 color;
    float roughness;
    float metalic;
    float specular;
    vec3 emission;
} pbr_mat_data;
/// @brief Create a PBR material ON STACK
/// @param s The pbr shader to use
/// @param color The color
/// @param roughness The roughness [0, 1]
/// @param metalic The metalicness [0, 1]
/// @param specular The specular coeff [0, 1]
/// @param emission The emission
/// @return The newly created material
material createMaterial_PBR(shader s, vec4 color, float roughness, float metalic, float specular, vec3 emission);
/// @brief Create a new PBR material
/// @param s The pbr shader to use
/// @param color The color
/// @param roughness The roughness [0, 1]
/// @param metalic The metalicness [0, 1]
/// @param specular The specular coeff [0, 1]
/// @param emission The emission
/// @return The newly created material
material* newMaterial_PBR(shader s, vec4 color, float roughness, float metalic, float specular, vec3 emission);



typedef struct PBRTexturedMaterialData {
    pbr_mat_data stdData;

    texture2D* albedo;
    texture2D* normalMap;
    texture2D* RMSMap;
    texture2D* emissionMap;
} pbr_tex_mat_data;
/// @brief Create a PBR material with textures ON STACK
/// @param s The pbr shader to use
/// @param color The color
/// @param roughness The roughness [0, 1]
/// @param metalic The metalicness [0, 1]
/// @param specular The specular coeff [0, 1]
/// @param emission The emission
/// @param albedoMap The albedo map
/// @param normalMap The normal map
/// @param roughnessMetalicSpecularMap The texture with (RGB) = (roughness, Metalic, Specular)
/// @param emissionMap The emmission map
/// @return The newly created material
material createMaterial_PBRTextured(shader s, 
    vec4 color, float roughness, float metalic, float specular, vec3 emission,
    texture2D* albedoMap, texture2D* normalMap, texture2D* roughnessMetalicSpecularMap,
    texture2D* emissionMap
);
/// @brief Create a new PBR material with textures
/// @param s The pbr shader to use
/// @param color The color
/// @param roughness The roughness [0, 1]
/// @param metalic The metalicness [0, 1]
/// @param specular The specular coeff [0, 1]
/// @param emission The emission
/// @param albedoMap The albedo map
/// @param normalMap The normal map
/// @param roughnessMetalicSpecularMap The texture with (RGB) = (roughness, Metalic, Specular)
/// @param emissionMap The emmission map
/// @return The newly created material
material* newMaterial_PBRTextured(shader s, 
    vec4 color, float roughness, float metalic, float specular, vec3 emission,
    texture2D* albedoMap, texture2D* normalMap, texture2D* roughnessMetalicSpecularMap,
    texture2D* emissionMap
);

#endif