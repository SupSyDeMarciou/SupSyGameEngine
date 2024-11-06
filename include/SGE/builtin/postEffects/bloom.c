#include "blur.h"

static post_shader bloom = 0;
bool initializeBloomLogic() {
    if (bloom) return false;
    initializeBlurLogic();
    bloom = createPostProcessShader("!builtin/postEffects/bloom.fs");
    return true;
}
void terminateBloomLogic() {
    freeShader(bloom);
}

void blitBloom(texture2D* source, frame_buffer* dest, uint nbIter, float intensity) {

    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 1, -1, "SGE Applying Bloom");

    if (nbIter > SGE_BUILTIN_BLUR_MAX_ITER) failWithError("Cannot apply so many blur iterations (max is %d)", SGE_BUILTIN_BLUR_MAX_ITER);

    shaderSetBool(bloom, "Pass", 0);
    const texture2D* blured = applyBlur(source, bloom, nbIter);

    shaderSetInt(bloom, "NbIter", nbIter);
    shaderSetBool(bloom, "Pass", 1);
    shaderSetFloat(bloom, "Intensity", intensity);
    shaderSetTexture2D(bloom, "Blured", 1, blured);
    blit(bloom, source, dest);

    glPopDebugGroup();
}

void blitBloomFB(frame_buffer* source, frame_buffer* dest, uint nbIter, float intensity) {
    blitBloom(FBGetColor(source, 0), dest, nbIter, intensity);
}