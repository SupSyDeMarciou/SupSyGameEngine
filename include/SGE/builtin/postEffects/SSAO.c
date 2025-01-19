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
    if (!isFBComplete(ssaoTemp)) SGE_fail("SSAO - Failed frame buffer initialization");

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
    if (distance < 0.0) SGE_fail("SSAO - Distance can't be negative!");
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

    blitFB(SSAO, source, ssaoTemp);
    texture2D* blured = FBGetColor(ssaoTemp, 0);
    // const texture2D* blured = applyBlur(FBGetColor(source, 0), SSAO, 2);
    
    shaderSetBool(SSAO, "Pass", true);
    shaderSetTexture2D(SSAO, "Depth", 1, blured);
    blitFB(SSAO, source, dest);

    glPopDebugGroup();
}