#include <SupSy/SGE.h>

static uint width = SGE_BASE_WIDTH, height = SGE_BASE_HEIGHT;
void renderFractal(comp_shader fractalShader, texture2D* destination, vec2 pos, float ezoom) {

    shaderUse(fractalShader);
    shaderSetTexture2D_Image(fractalShader, "Texture", 0, destination, GL_WRITE_ONLY);
    shaderSetVec2(fractalShader, "u_Pos", &pos);
    shaderSetFloat(fractalShader, "u_Zoom", ezoom);

    compShaderDispatch(fractalShader, width / 8 + ((width % 8) != 0), height / 8 + ((height % 8) != 0), 1);
}

#define ZOOM_SPEED 0.5
#define TRANSLATION_SPEED 2.0
int main(int argc, const char* argv[]) {

    // Call "./main [path to shader]" or "./main [x resolution] [y resolution] [path to shader]"

    if (argc < 1) failWithError("Must provide path to shader and eventually image size", 0);
    if (argc > 2) {
        width = SL_isNum(argv[1][0]) ? SL_readNum(argv[1], 0) : SGE_BASE_WIDTH;
        height = SL_isNum(argv[2][0]) ? SL_readNum(argv[2], 0) : SGE_BASE_HEIGHT;
    }

    initializeApp("Fractal visualizer");

    comp_shader shader = createComputeShader(argv[argc > 2 ? 3 : 1]);
    shaderSetUint2(shader, "u_ImageSize", width, height);
    texture2D* texture = newTexture2D(Uvec2(width, height), tex2DGenFormat(TEX_COL_RGBA, TEX_BIT_DEPTH_32, TEX_FORM_FLOAT));
    // tex2DSetFilter(texture, TEX_FILTER_NEAREST, TEX_FILTER_NEAREST);
    // tex2DApplyChanges(texture);

    float zoom = 1.0;
    vec2 pos = Vec2(0, 0);
    while (!appShouldClose()) {
        startFrameUpdate();
        if (inputIsKeyPressed(SGE_KEY_ESCAPE)) appSetShouldClose(true);

        float fast = (1.0 + inputIsKeyDown(SGE_KEY_SHIFT));
        zoom += (inputIsKeyDown(SGE_KEY_A) - inputIsKeyDown(SGE_KEY_E)) * ZOOM_SPEED * fast * TIME.dt;
        float ezoom = exp(-zoom);
        pos = addS2(pos, Vec2(inputIsKeyDown(SGE_KEY_D) - inputIsKeyDown(SGE_KEY_Q), inputIsKeyDown(SGE_KEY_S) - inputIsKeyDown(SGE_KEY_Z)), TRANSLATION_SPEED * fast * TIME.dt * ezoom);

        renderFractal(shader, texture, pos, ezoom);
        blitToScreen(texture);
        endFrameUpdate();
    }

    freeTexture2D(texture);
    destroyShader(shader);

    destroyApp();
    return 0;
}