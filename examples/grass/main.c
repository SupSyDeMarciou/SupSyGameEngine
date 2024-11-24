#define SGE_BASE_WIDTH (1920 * 4)
#define SGE_BASE_HEIGHT (1080 * 4)

#include <SGE.h>
#include <SGE/builtin/extData/freeCam.h>
#include <SGE/builtin/postEffects/bloom.h>
#include <SGE/builtin/postEffects/ssao.h>

int main() {

    // Initialization + Registration
    initializeApp("Grass");

    initializeBloom();
    initializeSSAO();
    
    SSAOSetDistance(0.1);
    registerFreeCam();

    // Creation of the noise texture for the floor
    const uint noiseTexRes = 4096;
    texture2D* noise = newTexture2D(uvec2One(noiseTexRes), tex2DGenFormat(TEX_COL_RGBA, TEX_BIT_DEPTH_16, TEX_FORM_FLOAT));
    tex2DSetWrap(noise, TEX_WRAP_MIRRORED, TEX_WRAP_MIRRORED);
    tex2DApplyChanges(noise);
    
    // Filling the noise texture
    const float noiseStrength = 5.0;
    comp_shader perlin = createComputeShader("perlin.cs");
    shaderSetInt(perlin, "u_Resolution", noiseTexRes);
    shaderSetFloat(perlin, "u_Scale", 1);
    shaderSetFloat(perlin, "u_Detail", 16.0);
    shaderSetFloat(perlin, "u_Roughness", 0.35);
    shaderSetFloat(perlin, "u_Distorsion", 1.0);
    shaderSetTexture2D_Image(perlin, "Texture", 0, noise, GL_WRITE_ONLY);
    const uint trueDispatchCount = noiseTexRes / 8 + (noiseTexRes % 8 != 0);
    compShaderDispatch(perlin, trueDispatchCount, trueDispatchCount, 1);

    // Create grass shader
    shader s = createShader("displace.vs", "!builtin/shaders/pbr.fs", NULL);
    shaderSetTexture2D(s, "u_DisplacementMap", 0, noise);
    shaderSetFloat(s, "u_Strength", noiseStrength);
    shaderAttachUniformBufferBP(s, SGE_SBU_BP_Camera);
    shaderAttachUniformBufferBP(s, SGE_SBU_BP_Object);

    // Create floor (on which to instance grass)
    const uint gridRes = 50;
    const float gridSize = 50;
    sc_obj* ground = newSceneObject(vec3_zero, quat_identity, Vec3(gridSize, 1, gridSize), NULL, true, NULL);
    material* groundMat = newMaterial_PBRTextured(s, Vec4(0.025, 0.015, 0.01, 1.0), 1.0, 0.0, 0.0, vec3_zero, noise, NULL, NULL, NULL);
    scobjAttachRenderObject_SingleMat(ground, meshUnitGrid(gridRes, gridRes), groundMat, true, RENDER_CULL_FRONT, true, NULL, NULL);

    // Generate grass shader
    const uint nbBlades = 1000;
    shader grassShader = createShader("grass.vs", "grass.fs", NULL);
    shaderSetTexture2D(grassShader, "u_DisplacementMap", 0, noise);
    shaderSetFloat(grassShader, "u_Strength", noiseStrength);
    shaderSetFloat(grassShader, "u_Size", gridSize);
    shaderSetInt(grassShader, "u_Count", nbBlades);
    shaderSetFloat4(grassShader, "mColor", 0.1, 1, 0.3, 0);
    shaderAttachUniformBufferBP(grassShader, SGE_SBU_BP_Camera);
    shaderAttachUniformBufferBP(grassShader, SGE_SBU_BP_Environment);

    // Generate grass blade and data
    const float directionRandomness = 0.33;
    const float scaleRandomness = 0.05;
    mesh* grassMesh = meshTransform(meshUnitGrid(1, 3), Vec3(0, 0.5, 0), Quat_Euler(0.0, PI/2, 0.0), Vec3(0.05, 1.0, 1.0));
    GLuint grassVbo; glGenBuffers(1, &grassVbo);
    meshAddVertexAttribute_Float(grassMesh, grassVbo, 4, 4, false, GL_FLOAT, sizeof(vec4), 0, 1);
    vec4* grassData = malloc(sizeof(vec4) * nbBlades * nbBlades);
    for (uint i = 0; i < nbBlades * nbBlades; i++) {
        vec2 v = rand2();
        grassData[i].x = v.x;
        grassData[i].y = v.y;
        grassData[i].z = SL_lerp(1.0, SL_randFloat(), scaleRandomness);
        grassData[i].w = SL_lerp(30 * DEG_TO_RAD, SL_randFloat() * PI, directionRandomness);
    }
    glBindBuffer(GL_ARRAY_BUFFER, grassVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * nbBlades * nbBlades, grassData, GL_STATIC_DRAW);

    // Create fog shader
    shader fog = createPostProcessShader("fog.fs");
    shaderAttachUniformBufferBP(fog, SGE_SBU_BP_Camera);
    shaderSetFloat(fog, "u_Intensity", 1.0);
    shaderSetFloat(fog, "u_FogDepth", 80.0);

    // Add simple cube
    sc_obj* cube = newSceneObject(Vec3(0, 1, 0), quat_identity, vec3One(1), NULL, true, NULL);
    shader pbr = createSurfaceShader("!builtin/shaders/pbr.fs");
    material* cubeMat = newMaterial_PBR(pbr, vec4_one, 1.0, 0.0, 0.001, vec3_zero);
    scobjAttachRenderObject_SingleMat(cube, meshUnitCube(), cubeMat, true, RENDER_CULL_FRONT, true, NULL, NULL);

    // Add camera
    sc_obj* camera = freeCam_addDefault(vec3_zero, quat_identity, 60*DEG_TO_RAD);
    camSetFCP(scobjGetExtData(camera, cam), 1000);
    RESetRenderCamera(scobjGetExtData(camera, cam));

    // Add sun
    sc_obj* sun = newSceneObject(vec3_zero, Quat_Euler(250*DEG_TO_RAD, -30*DEG_TO_RAD, 0), vec3One(1), NULL, false, NULL);
    scobjAttachLight_Directional(sun, scale3(Vec3(1.0, 1.0, 0.95), 15.0));
    REbackground_skySetSun(scobjGetExtData(sun, light));

    // Create secondary buffer for blit operations
    frame_buffer* fb = newFrameBuffer(Uvec2(SGE_BASE_WIDTH, SGE_BASE_HEIGHT));
    FBAttachColor(fb, tex2DGenFormat(TEX_COL_RGBA, TEX_BIT_DEPTH_16, TEX_FORM_FLOAT));
    FBAttachDepthStencil(fb, FB_DS_D16);
    FBFailIfNotComplete(fb);

    frame_buffer* fb0 = REGetOutputFB(APP->renderEnvironment);

    while (!appShouldClose()) {
        startFrameUpdate();
        if (inputIsKeyPressed(SGE_KEY_ESCAPE)) appSetShouldClose(true);

        sceneUpdate();

        RERenderScene();
        glDisable(GL_CULL_FACE); // grass mesh is a thin blade, it has no depth
        shaderSetFloat(grassShader, "u_Time", TIME.timeAtFrameStart);
        meshRenderInstanced(grassMesh, grassShader, nbBlades * nbBlades);

        blitSSAOFB(fb0, fb);
        shaderSetFBDepthStencil(fog, fb0, "Depth", 1);
        blitFB(fog, fb, fb0);
        
        blitBloomFB(fb0, fb, 8, 0.5);
        blitHdrToLdrFB(fb, fb0);
        blitToScreenFB(fb0);

        endFrameUpdate();
    }
    
    destroyApp();
    return 0;
}