#include "buffers.h"

GLint SGE_SHADER_BUFFER_UNIFORM_OFFSET_ALIGNMENT = 0;
uint SBUCalcAlignment(uint i) {
    return ((i + (uint)SGE_SHADER_BUFFER_UNIFORM_OFFSET_ALIGNMENT - 1) / (uint)SGE_SHADER_BUFFER_UNIFORM_OFFSET_ALIGNMENT) * (uint)SGE_SHADER_BUFFER_UNIFORM_OFFSET_ALIGNMENT;
}

shader_buff_uni* newShaderBufferUniform() {
    if (SGE_SHADER_BUFFER_UNIFORM_OFFSET_ALIGNMENT == 0) {
        glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, (GLint*)&SGE_SHADER_BUFFER_UNIFORM_OFFSET_ALIGNMENT);
        printf("Shader buffer uniform offset alignment = %d\n", SGE_SHADER_BUFFER_UNIFORM_OFFSET_ALIGNMENT);
    } 

    shader_buff_uni* sbu = malloc(sizeof(shader_buff_uni));
    if (!sbu) failWithError("Couldn't allocate SBU!");
    sbu->size = 0;
    glGenBuffers(1, &sbu->ubo);
    sbu->bindings = createList(1);
    sbu->finalized = false;

    return sbu;
}
void freeShaderBufferUniform(shader_buff_uni* toFree) {
    if (!toFree) return;
    glDeleteBuffers(1, &toFree->ubo);
    destroyList(toFree->bindings);
    free(toFree);
}

sbu_bp* SBUAttachBindingPoint(shader_buff_uni* sbu, const char* name, uint bindIDX, uint size) {
    if (sbu->finalized) {
        throwWarning("[ACTION IGNORED] - Trying to bind \"%s\" at %d to an already finalized SBU!", name, bindIDX);
        return NULL;
    }

    uint offset = SBUCalcAlignment(sbu->size);
    listAddLast(&sbu->bindings, ((sbu_bp){.parent = sbu, .name = name, .bindingIDX = bindIDX, .size = size, .offset = offset}));
    sbu->size = offset + size;

    sbu_bp* ret = n_elem_p(sbu_bp, sbu->bindings.last);
    printf("Attached \"%s\" at %u of size %u and offset %u\n", ret->name, ret->bindingIDX, ret->size, ret->offset);

    return n_elem_p(sbu_bp, sbu->bindings.last);
}

void SBUFinalize(shader_buff_uni* sbu) {
    if (sbu->finalized) return;
    sbu->finalized = true;

    glBindBuffer(GL_UNIFORM_BUFFER, sbu->ubo);
    glBufferData(GL_UNIFORM_BUFFER, sbu->size, NULL, GL_DYNAMIC_DRAW);

    list_foreach_t(&sbu->bindings, sbu_bp, bind) glBindBufferRange(GL_UNIFORM_BUFFER, bind->bindingIDX, sbu->ubo, bind->offset, bind->size);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void SBUSetData_Partial(const shader_buff_uni* sbu, void* data, uint offset, uint size) {
    if (!sbu->finalized) failWithError("Writting to non-finalized Shader Buffer Uniform!");
    if (offset + size > sbu->size) failWithError("Writting outside of SBU's reserved memory!");
    glBindBuffer(GL_UNIFORM_BUFFER, sbu->ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
void SBUSetData(const shader_buff_uni* sbu, void* data) {
    SBUSetData_Partial(sbu, data, 0, sbu->size);
}
void SBU_BPSetData_Partial(const sbu_bp* bp, void* data, uint offset, uint size) {
    if (offset + size > bp->size) failWithError("Writting outside of binding point \"%s\"'s reserved memory!", bp->name);
    SBUSetData_Partial(bp->parent, data, bp->offset + offset, size);
}
void SBU_BPSetData(const sbu_bp* bp, void* data) {
    SBU_BPSetData_Partial(bp, data, 0, bp->size);
}



GLint SGE_SHADER_BUFFER_STORAGE_OFFSET_ALIGNMENT = 0;
uint SBSCalcAlignment(uint i) {
    return ((i + (uint)SGE_SHADER_BUFFER_STORAGE_OFFSET_ALIGNMENT - 1) / (uint)SGE_SHADER_BUFFER_STORAGE_OFFSET_ALIGNMENT) * (uint)SGE_SHADER_BUFFER_STORAGE_OFFSET_ALIGNMENT;
}
shader_buff_sto* newShaderBufferStorage() {
    if (SGE_SHADER_BUFFER_STORAGE_OFFSET_ALIGNMENT == 0) {
        glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, (GLint*)&SGE_SHADER_BUFFER_STORAGE_OFFSET_ALIGNMENT);
        printf("Shader buffer storage offset alignment = %d\n", SGE_SHADER_BUFFER_STORAGE_OFFSET_ALIGNMENT);
    } 

    shader_buff_sto* sbs = malloc(sizeof(shader_buff_sto));
    if (!sbs) failWithError("Couldn't allocate Shader Buffer Storage!");
    sbs->size = 0;
    glGenBuffers(1, &sbs->ubo);
    sbs->bindings = createList(1);
    sbs->finalized = false;

    return sbs;
}
void freeShaderBufferStorage(shader_buff_sto* toFree) {
    if (!toFree) return;
    glDeleteBuffers(1, &toFree->ubo);
    destroyList(toFree->bindings);
    free(toFree);
}

sbs_bp* SBSAttachBindingPoint(shader_buff_sto* sbs, const char* name, uint bindIDX, uint size) {
    if (sbs->finalized) {
        throwWarning("[ACTION IGNORED] - Trying to bind \"%s\" at %d to an already finalized SBU!", name, bindIDX);
        return NULL;
    }

    uint offset = SBUCalcAlignment(sbs->size);
    listAddLast(&sbs->bindings, ((sbs_bp){.parent = sbs, .name = name, .bindingIDX = bindIDX, .size = size, .offset = offset}));
    sbs->size = offset + size;

    sbs_bp* ret = n_elem_p(sbs_bp, sbs->bindings.last);
    // printf("Attached \"%s\" at %d of size %d and offset %d\n", ret->name, ret->bindingIDX, ret->size, ret->offset);

    return n_elem_p(sbs_bp, sbs->bindings.last);
}

void SBSFinalize(shader_buff_sto* sbs) {
    if (sbs->finalized) return;
    sbs->finalized = true;

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, sbs->ubo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sbs->size, NULL, GL_DYNAMIC_DRAW);

    list_foreach_t(&sbs->bindings, sbs_bp, bind) glBindBufferRange(GL_SHADER_STORAGE_BUFFER, bind->bindingIDX, sbs->ubo, bind->offset, bind->size);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void SBSSetData_Partial(const shader_buff_sto* sbs, void* data, uint offset, uint size) {
    if (!sbs->finalized) failWithError("Writting to non-finalized SBS!");
    if (offset + size > sbs->size) failWithError("Writting outside of SBS's reserved memory!");
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, sbs->ubo);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
void SBSSetData(const shader_buff_sto* sbs, void* data) {
    SBSSetData_Partial(sbs, data, 0, sbs->size);
}
void SBS_BPSetData_Partial(const sbs_bp* bp, void* data, uint offset, uint size) {
    if (offset + size > bp->size) failWithError("Writting outside of binding point \"%s\"'s reserved memory!", bp->name);
    SBSSetData_Partial(bp->parent, data, bp->offset + offset, size);
}
void SBS_BPSetData(const sbs_bp* bp, void* data) {
    SBS_BPSetData_Partial(bp, data, 0, bp->size);
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////// * * *  BUILTIN BUFFERS  * * * ///////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



sbu_bp* SGE_SBU_BP_Environment = NULL;
sbu_bp* SGE_SBU_BP_Camera = NULL;
sbu_bp* SGE_SBU_BP_Object = NULL;