#ifndef __SGE_RENDER_RENDER_OBJECT_H__
#define __SGE_RENDER_RENDER_OBJECT_H__

#include "../scene/sc_object.h"
#include "mesh.h"
#include "material.h"
#include "render.h"

/// @brief Function just before an object is rendered
/// @param obj The object to render
typedef void func_onBeforeRender(sc_obj* obj);
/// @brief Function just after an object has been rendered
/// @param obj The object rendered
typedef void func_onAfterRender(sc_obj* obj);

/// @brief Render object external data structure
typedef struct RenderObject {
    mesh* mesh;
    array(material, p) materials;

    func_onBeforeRender* onBeforeRender;
    func_onAfterRender* onAfterRender;

    union {
        struct {
            bool render : 1;
            render_cull_face cull : 2;
            bool castShadow : 1;
            bool transparent : 1;
        };
        uint8 flags;
    };
} render_obj;
DEF_EXT_ID(render_obj)
SL_DEFINE_LIST(render_obj);

/// @brief Add a new render object external data block
/// @param source The scene object on which this data is to be added
/// @param mesh The mesh of the object
/// @param materials The materials with which to render
/// @param render If the object should render
/// @param cull How the object should cull
/// @param castShadow If the object casts shadows
/// @param transparent If the object has transparency
/// @param onBeforeRender Function called just before a render
/// @param onAfterRender Function called just after a render
render_obj* scobjAttachRenderObject (
    sc_obj* source,
    mesh* mesh, array(material, p) materials,
    bool render, render_cull_face cull, bool castShadow, bool transparent,
    func_onBeforeRender* onBeforeRender, func_onAfterRender* onAfterRender
);
/// @brief Add a new render object external data block
/// @param source The scene object on which this data is to be added
/// @param mesh The mesh of the object
/// @param material The material with which to render
/// @param render If the object should render
/// @param cull How the object should cull
/// @param castShadow If the object casts shadows
/// @param transparent If the object has transparency
/// @param onBeforeRender Function called just before a render
/// @param onAfterRender Function called just after a render
render_obj* scobjAttachRenderObject_SingleMat (
    sc_obj* source,
    mesh* mesh, material* material,
    bool render, render_cull_face cull, bool castShadow, bool transparent,
    func_onBeforeRender* onBeforeRender, func_onAfterRender* onAfterRender
);

/// @brief Get the scene object to which this renderObject is attached
/// @param ro The render object
/// @return The source scene object
sc_obj* ROGetScObj(render_obj* ro);

/// @brief Calculate necessary data for render
/// @param ro The object which needs calculating
void ROPrepareForRender(render_obj* ro);

/// @brief Render an object in current context
/// @param ro The object to be rendered
void RORender(render_obj* ro);

/// @brief If an object is visible by the camera
/// @param ro The object to check
/// @param camera The camera from which to check
/// @return If the object is visible by the camera
bool ROIsInCamFrustum(render_obj* ro, cam* camera);


#endif