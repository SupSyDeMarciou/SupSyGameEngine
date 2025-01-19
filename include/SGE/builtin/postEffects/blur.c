#include "blur.h"

#define SGE_BUILTIN_BLUR_BASE_SIZE 1
#define SGE_BUILTIN_BLUR_STEP 2

static comp_shader blur = 0;
static post_shader blurAdd = 0;
static frame_buffer* resizeRb[SGE_BUILTIN_BLUR_MAX_ITER + 2] = {NULL};

bool initializeBlur() {
    if (blur) return false;
    blur = createComputeShader("!builtin/postEffects/blur.cs");
    blurAdd = createPostProcessShader("!builtin/postEffects/blur_add.fs");

    tex_form format = tex2DGenFormat(TEX_COL_RGBA, TEX_BIT_DEPTH_16, TEX_FORM_FLOAT);

    uint width = SGE_BASE_WIDTH > 1920 ? 1920 : SGE_BASE_WIDTH;
    uint height = SGE_BASE_HEIGHT > 1080 ? 1080 : SGE_BASE_HEIGHT;

    width /= SGE_BUILTIN_BLUR_BASE_SIZE;
    height /= SGE_BUILTIN_BLUR_BASE_SIZE;
    
    for (int i = 0, s = 1; i < SGE_BUILTIN_BLUR_MAX_ITER; i++, s *= SGE_BUILTIN_BLUR_STEP) {
        resizeRb[i] = newFrameBuffer(Uvec2(width / s, height / s));
        FBAttachColor(resizeRb[i], format);
        FBAttachColor(resizeRb[i], format);
        if (!isFBComplete(resizeRb[i])) SGE_fail("BLUR - failed frame buffer initialization");
    }

    resizeRb[SGE_BUILTIN_BLUR_MAX_ITER] = newFrameBuffer(Uvec2(SGE_BASE_WIDTH, SGE_BASE_HEIGHT));
    FBAttachColor(resizeRb[SGE_BUILTIN_BLUR_MAX_ITER], format);
    if (!isFBComplete(resizeRb[SGE_BUILTIN_BLUR_MAX_ITER])) SGE_fail("BLUR - failed frame buffer initialization");

    resizeRb[SGE_BUILTIN_BLUR_MAX_ITER + 1] = newFrameBuffer(Uvec2(SGE_BASE_WIDTH, SGE_BASE_HEIGHT));
    FBAttachColor(resizeRb[SGE_BUILTIN_BLUR_MAX_ITER + 1], format);
    if (!isFBComplete(resizeRb[SGE_BUILTIN_BLUR_MAX_ITER + 1])) SGE_fail("BLUR - failed frame buffer initialization");
    return true;
}

static void applyBlurOnce(frame_buffer* buffer) {
    GLuint tex1 = FBGetColorGlID(buffer, 0);
    GLuint tex2 = FBGetColorGlID(buffer, 1);

    uvec2 bufferSize = FBGetSize(buffer);
    uint width = bufferSize.x / 8 + (bufferSize.x % 8 != 0);
    uint height = bufferSize.y / 8 + (bufferSize.y % 8 != 0);
    // uint format = tex2DGetFormat(tex1).format;
    uint format = GL_RGBA16F;

    shaderUse(blur);
    
    glBindImageTexture(0, tex1, 0, GL_FALSE, 0, GL_READ_WRITE, format);
    glBindImageTexture(1, tex2, 0, GL_FALSE, 0, GL_READ_WRITE, format);
    shaderSetBool(blur, "Pass", 0);
    compShaderDispatch(blur, width, height, 1);
    shaderSetBool(blur, "Pass", 1);
    compShaderDispatch(blur, width, height, 1);
}

const texture2D* applyBlur(texture2D* source, post_shader init, uint iter) {

    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 2, -1, "SGE Applying Blur");

    if (iter > SGE_BUILTIN_BLUR_MAX_ITER) {
        SGE_throwWarning("BLUR - Number of iterations limited to %d", SGE_BUILTIN_BLUR_MAX_ITER);
        iter = SGE_BUILTIN_BLUR_MAX_ITER;
    }

    if (iter == 0) {
        blit(init, source, resizeRb[0]);
        return FBGetColor(resizeRb[0], 0);
    }

    blit(init, source, resizeRb[0]);

    int pingPong = 0;
    for (int i = 0; i < iter; i++) {
        for (int j = 0; j <= i; j++) {
            applyBlurOnce(resizeRb[i]);
        }

        shaderSetInt(blurAdd, "Total", iter);
        shaderSetInt(blurAdd, "Pass", i);
        shaderSetFBColor(blurAdd, resizeRb[SGE_BUILTIN_BLUR_MAX_ITER + 1 - pingPong], "Self", 1, 0);
        blit(blurAdd, FBGetColor(resizeRb[i], 0), resizeRb[SGE_BUILTIN_BLUR_MAX_ITER + pingPong]);
        pingPong = !pingPong;

        if (i == iter - 1) break;
        blit(noOpp, FBGetColor(resizeRb[i], 0), resizeRb[i+1]);
    }

    glPopDebugGroup();

    return FBGetColor(resizeRb[SGE_BUILTIN_BLUR_MAX_ITER + 1 - pingPong], 0);
}