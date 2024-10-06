#ifndef __SGE_RENDERING_SHADERS_SHADER
#define __SGE_RENDERING_SHADERS_SHADER

#include "../../SGEstructures.h"

/// @brief Set the default path from which to look for shaders when only giving their file names
/// @param folder The path to the shader folder
void shaderSetDefaultPath(const char* folder);

/// @brief The unique ID of the shader program
typedef GLuint shader;

/// @brief Create a shader program from code
/// @param vertexShaderFilePath Path to vertex shader (Mandatory)
/// @param fragmentShaderFilePath Path to fragment shader (Mandatory)
/// @param geometryShaderFilePath Path to geometry shader (Set NULL to omit)
/// @return The compiled shader program
shader createShader(const char* vertexShaderFilePath, const char* fragmentShaderFilePath, const char* geometryShaderFilePath);

/// @brief Free a shader
/// @param toFree The shader to free
void freeShader(shader toFree);

/// @brief Create a shader for surface material
/// @param fragmentShaderFilePath Path to fragment shader
/// @return The compiled shader program
shader createSurfaceShader(const char* fragmentShaderFilePath);

/// @brief Use the shader for the following operations
/// @param s The shader ID
void shaderUse(shader s);

/// @brief Attach a uniform buffer block to a shader
/// @param s The shader
/// @param bufferName The name of the uniform buffer
/// @param bindingPoint The binding point to the uniform buffer
void shaderAttachUniformBuffer(shader s, const char* bufferName, uint bindingPoint);
/// @brief Attach a shader storage block to a shader
/// @param s The shader
/// @param bufferName The name of the shader storage
/// @param bindingPoint The binding point to the shader storage
void shaderAttachShaderStorage(shader s, const char* storageName, uint bindingPoint);

/// @brief Set a shader boolean
/// @param s Shader ID
/// @param fieldName Field name of the variable to set
/// @param value The desired value
void shaderSetBool(shader s, const char* fieldName, bool value);

/// @brief Set a shader int
/// @param s Shader ID
/// @param fieldName Field name of the variable to set
/// @param value The desired value
void shaderSetInt(shader s, const char* fieldName, int value);
/// @brief Set a shader ivec2
/// @param s Shader ID
/// @param fieldName Field name of the variable to set
/// @param x The desired x component
/// @param y The desired y component
void shaderSetInt2(shader s, const char* fieldName, int x, int y);
/// @brief Set a shader ivec3
/// @param s Shader ID
/// @param fieldName Field name of the variable to set
/// @param x The desired x component
/// @param y The desired y component
/// @param z The desired z component
void shaderSetInt3(shader s, const char* fieldName, int x, int y, int z);
/// @brief Set a shader ivec4
/// @param s Shader ID
/// @param fieldName Field name of the variable to set
/// @param x The desired x component
/// @param y The desired y component
/// @param z The desired z component
/// @param w The desired w component
void shaderSetInt4(shader s, const char* fieldName, int x, int y, int z, int w);

/// @brief Set a shader uint
/// @param s Shader ID
/// @param fieldName Field name of the variable to set
/// @param value The desired value
void shaderSetUint(shader s, const char* fieldName, uint value);
/// @brief Set a shader uvec2
/// @param s Shader ID
/// @param fieldName Field name of the variable to set
/// @param x The desired x component
/// @param y The desired y component
void shaderSetUint2(shader s, const char* fieldName, uint x, uint y);
/// @brief Set a shader uvec3
/// @param s Shader ID
/// @param fieldName Field name of the variable to set
/// @param x The desired x component
/// @param y The desired y component
/// @param z The desired z component
void shaderSetUint3(shader s, const char* fieldName, uint x, uint y, uint z);
/// @brief Set a shader uvec4
/// @param s Shader ID
/// @param fieldName Field name of the variable to set
/// @param x The desired x component
/// @param y The desired y component
/// @param z The desired z component
/// @param w The desired w component
void shaderSetUint4(shader s, const char* fieldName, uint x, uint y, uint z, uint w);

/// @brief Set a shader float
/// @param s Shader ID
/// @param fieldName Field name of the variable to set
/// @param value The desired value
void shaderSetFloat(shader s, const char* fieldName, float value);
/// @brief Set a shader vec2
/// @param s Shader ID
/// @param fieldName Field name of the variable to set
/// @param x The desired x component
/// @param y The desired y component
void shaderSetFloat2(shader s, const char* fieldName, float x, float y);
/// @brief Set a shader vec3
/// @param s Shader ID
/// @param fieldName Field name of the variable to set
/// @param x The desired x component
/// @param y The desired y component
/// @param z The desired z component
void shaderSetFloat3(shader s, const char* fieldName, float x, float y, float z);
/// @brief Set a shader vec4
/// @param s Shader ID
/// @param fieldName Field name of the variable to set
/// @param x The desired x component
/// @param y The desired y component
/// @param z The desired z component
/// @param w The desired w component
void shaderSetFloat4(shader s, const char* fieldName, float x, float y, float z, float w);

/// @brief Set a shader vec2
/// @param s Shader ID
/// @param fieldName Field name of the variable to set
/// @param v The desired vector
void shaderSetVec2(shader s, const char* fieldName, const vec2* v);
/// @brief Set a shader vec3
/// @param s Shader ID
/// @param fieldName Field name of the variable to set
/// @param v The desired vector
void shaderSetVec3(shader s, const char* fieldName, const vec3* v);
/// @brief Set a shader vec4
/// @param s Shader ID
/// @param fieldName Field name of the variable to set
/// @param v The desired vector
void shaderSetVec4(shader s, const char* fieldName, const vec4* v);

/// @brief Set a shader vec4 from quaternion
/// @param s Shader ID
/// @param fieldName Field name of the variable to set
/// @param q The desired quaternion
void shaderSetQuat(shader s, const char* fieldName, const quat* q);

/// @brief Set a shader mat2x2
/// @param s Shader ID
/// @param fieldName Field name of the variable to set
/// @param m The desired matrix
/// @param transpose If the matrix should be transposed
void shaderSetMat2x2(shader s, const char* fieldName, const mat2x2* m, bool transpose);
/// @brief Set a shader mat3x3
/// @param s Shader ID
/// @param fieldName Field name of the variable to set
/// @param m The desired matrix
/// @param transpose If the matrix should be transposed
void shaderSetMat3x3(shader s, const char* fieldName, const mat3x3* m, bool transpose);
/// @brief Set a shader mat4x4
/// @param s Shader ID
/// @param fieldName Field name of the variable to set
/// @param m The desired matrix
/// @param transpose If the matrix should be transposed
void shaderSetMat4x4(shader s, const char* fieldName, const mat4x4* m, bool transpose);



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////// * * *  COMPUTE SHADER  * * * ////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/// @brief Compute shader object
typedef shader comp_shader;

/// @brief Create a compute shader program from code
/// @param computeShaderFileName Path to compute shader
/// @return The compiled compute shader program
comp_shader createComputeShader(const char* computeShaderFileName);

/// @brief Dispatch the compute shader
/// @param s The shader ID
/// @param x The number of groups along x
/// @param y The number of groups along y
/// @param z The number of groups along z
void compShaderDispatch(comp_shader s, uint x, uint y, uint z);

#endif