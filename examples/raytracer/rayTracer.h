#ifndef __RAYTRACER_H__
#define __RAYTRACER_H__

#include <SGE.h>

/// @brief Have the raytracer accumulate frames indefinitly
#define RAYTRACER_NO_CAP -1
/// @brief Initialize RayTracer
/// @param result Where to render to
/// @param maxBounces Maximum number of times a ray may bounce
/// @param maxTracesPerFrame Maximum number of rays to shoot per pixel in a single frame
/// @param totalSamples Number of samples to work towrads when doing accumulation
/// @return Wether the RayTracer was initialized
bool initializeRayTracer(texture2D* result, uint maxBounces, uint maxTracesPerFrame, int totalSamples);

/// @brief Add an object to the RTScene
/// @param position Position of the object
/// @param radius Radius of the object
/// @param materialIdx Material index of this object
/// @return The index in RTScene of the added object
uint raytracerAddObject(vec3 position, float radius, uint materialIdx);
/// @brief Add a material to the RTScene
/// @param albedo Base color of the material
/// @param rougness Roughness of the material [0, 1]
/// @param specularCol Color of highlights of this material
/// @param specularProb Probability of highlights of this material
/// @param emission Color of the light emission of this material
/// @param emissionStrength Strength of the light emission of this material
/// @param transmission Wether this material acts like glass
/// @param IOR The index of refraction of this material
/// @return The index in RTScene of the added material
uint raytracerAddMaterial(vec3 albedo, float rougness, vec3 specularCol, float specularProb, vec3 emission, float emissionStrength, float transmission, float IOR);
/// @brief Convert an RTMaterial to a PBR material
/// @param surfaceShader Base pbr surface shader
/// @param rtMat Index of the RTMaterial
/// @return The newly created material
material* materialFromRTMat(shader surfaceShader, uint rtMat);

/// @brief Send the complete scene to the GPU
void raytracerSendScene();

/// @brief Render to the texture
void raytracerRender();
/// @brief Reset accumulation
void raytracerRestart();

/// @brief Set the number of ray bounces
/// @param n The new number of ray bounces
void raytracerSetNbBounces(uint n);
/// @brief Set the number of traces
/// @param n The new number of traces
void raytracerSetNbTraces(uint n);
/// @brief Get the accumulation count
/// @return The number of accumulated frames
uint raytracerGetCompletion();

#endif