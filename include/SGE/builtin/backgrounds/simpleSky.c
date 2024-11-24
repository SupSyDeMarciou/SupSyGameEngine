#include "simpleSky.h"

typedef struct Private_SimpleSky {
    simple_sky public;

    light* sun;
    vec3 sunColor, ambiantColor;
    post_shader shader;
} p_simple_sky;

simple_sky* newSimpleSky(vec3 skyColorDay, vec3 skyColorNoon, vec3 skyColorNight, vec3 sunColorDay, vec3 sunColorNoon, light* sun) {
    p_simple_sky* new = malloc(sizeof(p_simple_sky));
    new->public.skyColorDay = skyColorDay;
    new->public.skyColorNoon = skyColorNoon;
    new->public.skyColorNight = skyColorNight;
    new->public.sunColorDay = sunColorDay;
    new->public.sunColorNoon = sunColorNoon;
    new->sun = sun;
    new->ambiantColor = vec3_zero;
    new->sunColor = vec3_zero;
    new->shader = createPostProcessShader("!builtin/backgrounds/simpleSky.fs");
    printf("Created shader\n");

    return (simple_sky*)new;
}
void freeSimpleSky(simple_sky* toFree) {
    if (!toFree) return;

    p_simple_sky* p_sk = (p_simple_sky*)toFree;
    destroyShader(p_sk->shader);
    free(p_sk);
}
shader simpleSkyRender();
re_background simpleSkyAsREBackground(simple_sky* data) {
    return (re_background) {.data = data, .freeData = (func_free*)freeSimpleSky, .render = simpleSkyRender};
}

#define __SKY_TRANSITION 0.2
static void skyCalcTrueColors(p_simple_sky* p_sk) {

    float dot = -1.0;
    if (p_sk->sun) {
        quat rot = scobjWorldRot(lightGetScObj(p_sk->sun));
        vec3 sunDir; rot3Q_(&vec3_forward, &rot, &sunDir);
        dot = dot3(sunDir, vec3_down);
    }

    float t = __max(0, dot);
    float t1 = 1 - (t - __SKY_TRANSITION) / (1.0 - __SKY_TRANSITION);
    float t2 = 1 - t / __SKY_TRANSITION;

    if (t > __SKY_TRANSITION) {
        p_sk->ambiantColor = lerp3(p_sk->public.skyColorDay, p_sk->public.skyColorNoon, t1*t1);
        p_sk->sunColor = lerp3(p_sk->public.sunColorDay, p_sk->public.sunColorNoon, t1*t1);
    } else {
        p_sk->ambiantColor = lerp3(p_sk->public.skyColorNoon, p_sk->public.skyColorNight, t2);
        p_sk->sunColor = lerp3(p_sk->public.sunColorNoon, vec3_zero, t2);
    }

    RESetAmbiantColor(p_sk->ambiantColor);
    if (p_sk->sun) p_sk->sun->color = p_sk->sunColor;
}
light* simpleSkyGetSun(simple_sky* sk) {
    return ((p_simple_sky*)sk)->sun;
}
bool simpleSkySetSun(simple_sky* sk, light* l) {
    if (l && l->type != LIGHT_TYPE_DIRECTIONAL) return false;
    
    p_simple_sky* p_sk = (p_simple_sky*)sk;
    p_sk->sun = l;
    return true;
}
shader simpleSkyRender() {
    p_simple_sky* p_sk = (p_simple_sky*)REGetBackground();
    skyCalcTrueColors(p_sk);

    post_shader s = p_sk->shader;
    int idx = REGetLightIndex(APP->renderEnvironment, p_sk->sun);
    shaderSetUint(s, "u_SunIdx", idx >= 0 ? idx + 1 : 0);
    return s;
}