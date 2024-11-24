#include "blur.h"

#define SGE_SSAO_NB_RAND_VECT 256
static post_shader SSAO = -1;
static vec3 randVec[SGE_SSAO_NB_RAND_VECT] = {0};
static frame_buffer* ssaoTemp = NULL;
bool initializeSSAO() {
    if (SSAO != -1) return false;
    SSAO = createPostProcessShader("!builtin/postEffects/SSAO.fs");
    shaderAttachUniformBufferBP(SSAO, SGE_SBU_BP_Camera);
    
    ssaoTemp = newFrameBuffer(Uvec2(SGE_BASE_WIDTH, SGE_BASE_HEIGHT));
    FBAttachColor(ssaoTemp, tex2DGenFormat(TEX_COL_RGBA, TEX_BIT_DEPTH_16, TEX_FORM_FLOAT));
    if (!isFBComplete(ssaoTemp)) failWithError("Failed to initialize SSAO: incomplete framebuffer", 0);

    char pos[128] = {0};
    for (uint i = 0; i < SGE_SSAO_NB_RAND_VECT; i++) {
        randVec[i] = rand3();
        if (randVec[i].z < 0) randVec[i].z = -randVec[i].z;
        scale3_s(randVec + i, sqrt(SL_randFloat()));
        sprintf(pos, "RandVec[%d]", i);
        shaderSetVec3(SSAO, pos, &randVec[i]);
    }
    return true;
}
void terminateSSAOLogic() {
    freeFrameBuffer(ssaoTemp);
    destroyShader(SSAO);
}

void SSAOSetDistance(float distance) {
    if (distance < 0.0) failWithError("SSAO distance can't be negative!");
    shaderSetFloat(SSAO, "Radius", distance);
}
void SSAOSetIntensity(float intensity) {
    shaderSetFloat(SSAO, "Intensity", intensity);
}

void blitSSAOFB(frame_buffer* source, frame_buffer* dest) {

    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 1, -1, "SGE Applying SSAO");

    shaderSetBool(SSAO, "Pass", false);
    shaderSetFBColor(SSAO, source, "Normal", 1, 1);
    shaderSetFBDepthStencil(SSAO, source, "Depth", 2);

    blit(SSAO, FBGetColor(source, 0), ssaoTemp);
    texture2D* blured = FBGetColor(ssaoTemp, 0);
    // const texture2D* blured = applyBlur(FBGetColor(source, 0), SSAO, 2);
    
    shaderSetBool(SSAO, "Pass", true);
    shaderSetTexture2D(SSAO, "Depth", 1, blured);
    blit(SSAO, FBGetColor(source, 0), dest);

    glPopDebugGroup();
}