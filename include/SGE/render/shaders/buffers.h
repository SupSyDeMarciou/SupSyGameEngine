#ifndef __SGE_RENDER_SHADERS_BUFFERS_H__
#define __SGE_RENDER_SHADERS_BUFFERS_H__

#include "../../constants.h"

typedef struct ShaderBufferUniform shader_buff_uni;
typedef struct ShaderBufferUniform_BindingPoint {
    shader_buff_uni* parent;
    const char* name;
    uint bindingIDX;
    
    uint offset, size;
} sbu_bp;
struct ShaderBufferUniform {
    GLuint ubo;
    uint size;
    list bindings;

    bool finalized;
};
/// @brief Get the shader buffer uniform offset alignment on this machine
/// @return The shader buffer uniform offset alignment
uint SBUGetOffsetAlignment();
/// @brief Calculate the SBU alignment
/// @param i The position
/// @return The calculated alignment
uint SBUCalcAlignment(uint i);

/// @brief Create an empty shader buffer uniform object ON STACK
/// @return The newly created shader buffer uniform
/// @note This buffer must be finalized with "SBUFinalize" to sent to the GPU and thus be used
shader_buff_uni createShaderBufferUniform();
/// @brief Destroy a shader buffer uniform
/// @param toDestroy The shader buffer uniform to destroy
void destroyShaderBufferUniform(shader_buff_uni toDestroy);
/// @brief Create a new empty shader buffer uniform object
/// @return The newly created shader buffer uniform
/// @note This buffer must be finalized with "SBUFinalize" to sent to the GPU and thus be used
shader_buff_uni* newShaderBufferUniform();
/// @brief Free a shader buffer uniform
/// @param toFree The shader buffer uniform to free
void freeShaderBufferUniform(shader_buff_uni* toFree);

/// @brief Attach a new binding point to a SBU
/// @param sbu The SBU on which to attach
/// @param name The name of the binding point
/// @param bindIDX The index of the binding point
/// @param size The size of the memory reserved to this binding point
/// @return The newly added binding point
/// @note Attaching binding points may only happen while the SBU is not finalized 
sbu_bp* SBUAttachBindingPoint(shader_buff_uni* sbu, const char* name, uint bindIDX, uint size);

/// @brief Finalize an SBU to make it usable
/// @param sbu The SBU to finalize
void SBUFinalize(shader_buff_uni* sbu);

/// @brief Set part of the memory reserved to a SBU
/// @param sbu The SBU to write to
/// @param data The data to write
/// @param offset The offset relative to the start of the SBU
/// @param size The size of the data to write
void SBUSetData_Partial(const shader_buff_uni* sbu, void* data, uint offset, uint size);
/// @brief Set the entire memory reserved to a SBU
/// @param sbu The SBU to write to
/// @param data The data to write
void SBUSetData(const shader_buff_uni* sbu, void* data);
/// @brief Set part of the memory reserved to an SBU's binding point
/// @param bp The binding point to write to
/// @param data The data to write
/// @param offset The offset relative to the start of the binding point
/// @param size The size of the data to write
void SBU_BPSetData_Partial(const sbu_bp* bp, void* data, uint offset, uint size);
/// @brief Set the entire memory reserved to an SBU's binding point
/// @param bp The binding point to write to
/// @param data The data to write
void SBU_BPSetData(const sbu_bp* bp, void* data);



typedef struct ShaderBufferStorage shader_buff_sto;
typedef struct ShaderBufferStorage_BindingPoint {
    shader_buff_sto* parent;
    const char* name;
    uint bindingIDX;
    
    uint offset, size;
} sbs_bp;
struct ShaderBufferStorage {
    GLuint ubo;
    uint size;
    list bindings;

    bool finalized;
};
/// @brief Get the shader buffer storage offset alignment on this machine
/// @return The shader buffer storage offset alignment
uint SBSGetOffsetAlignment();
/// @brief Calculate the SBS alignment
/// @param i The position
/// @return The calculated alignment
uint SBSCalcAlignment(uint i);

/// @brief Create an empty shader buffer storage object ON STACK
/// @return The newly created shader buffer storage
/// @note This buffer must be finalized with "SBSFinalize" to sent to the GPU and thus be used
shader_buff_sto createShaderBufferStorage();
/// @brief Destroy a shader buffer storage
/// @param toDestroy The shader buffer storage to destroyed
void destroyShaderBufferStorage(shader_buff_sto toDestroy);
/// @brief Create a new empty shader buffer storage object
/// @return The newly created shader buffer storage
/// @note This buffer must be finalized with "SBSFinalize" to sent to the GPU and thus be used
shader_buff_sto* newShaderBufferStorage();
/// @brief Free a shader buffer storage
/// @param toFree The shader buffer storage to free
void freeShaderBufferStorage(shader_buff_sto* toFree);

/// @brief Attach a new binding point to a SBS
/// @param sbs The SBS on which to attach
/// @param name The name of the binding point
/// @param bindIDX The index of the binding point
/// @param size The size of the memory reserved to this binding point
/// @return The newly added binding point
/// @note Attaching binding points may only happen while the SBS is not finalized 
sbs_bp* SBSAttachBindingPoint(shader_buff_sto* sbs, const char* name, uint bindIDX, uint size);

/// @brief Finalize an SBS to make it usable
/// @param sbs The SBS to finalize
void SBSFinalize(shader_buff_sto* sbs);

/// @brief Set part of the memory reserved to a SBS
/// @param sbs The SBS to write to
/// @param data The data to write
/// @param offset The offset relative to the start of the SBS
/// @param size The size of the data to write
void SBSSetData_Partial(const shader_buff_sto* sbs, void* data, uint offset, uint size);
/// @brief Set the entire memory reserved to a SBS
/// @param sbs The SBS to write to
/// @param data The data to write
void SBSSetData(const shader_buff_sto* sbs, void* data);
/// @brief Set part of the memory reserved to an SBS's binding point
/// @param bp The binding point to write to
/// @param data The data to write
/// @param offset The offset relative to the start of the binding point
/// @param size The size of the data to write
void SBS_BPSetData_Partial(const sbs_bp* bp, void* data, uint offset, uint size);
/// @brief Set the entire memory reserved to an SBS's binding point
/// @param bp The binding point to write to
/// @param data The data to write
void SBS_BPSetData(const sbs_bp* bp, void* data);



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////// * * *  BUILTIN BUFFERS  * * * ///////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern sbu_bp* SGE_SBU_BP_Environment;
extern sbu_bp* SGE_SBU_BP_Camera;
extern sbu_bp* SGE_SBU_BP_Object;

#endif