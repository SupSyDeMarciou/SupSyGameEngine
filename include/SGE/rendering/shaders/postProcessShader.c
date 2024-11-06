#include "postProcessShader.h"
#include "../../application/application.h"

post_shader createPostProcessShader(const char* path) {
    return createShader("!rendering/shaders/screen.vs", path, NULL);
}

void blit(post_shader s, texture2D* source, frame_buffer* dest) {

    if (!dest) failWithError("Cannot blit to NULL render buffer!", 0);

    FBBind(dest, GL_COLOR_BUFFER_BIT, Vec4(1.0, 0.0, 0.0, 1.0));
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    shaderSetTexture2D(s, "Source", 0, source);
    RERenderQuad(APP->renderEnvironment, s);

    glActiveTexture(GL_TEXTURE0);
}

void blitFB(post_shader s, frame_buffer* source, frame_buffer* dest) {

    if (!source) failWithError("Cannot blit from NULL render buffer!");
    if (!dest) failWithError("Cannot blit to NULL render buffer!");

    FBBind(dest, GL_COLOR_BUFFER_BIT, Vec4(1.0, 0.0, 0.0, 1.0));
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    shaderSetFBColor(s, source, "Source", 0, 0);
    RERenderQuad(APP->renderEnvironment, s);

    // glActiveTexture(GL_TEXTURE0);
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////// * * *  PRE-MADE POST PROCESS SHADERS  * * * /////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


post_shader noOpp = -1;
void post_sInitNoOpp() {
    if (noOpp == -1) noOpp = createPostProcessShader("!rendering/shaders/no_opp.fs");
}