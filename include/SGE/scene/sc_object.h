#ifndef __SGE_SCENE_SC_OBJECT
#define __SGE_SCENE_SC_OBJECT

#include <stdarg.h>
#include "../SGEstructures.h"

#include "../rendering/mesh.h"
#include "../rendering/material.h"


#define EXT_ID_TRANSFORM 0
/// @brief Structure representing an object's spacial coordinates
typedef struct Transform trsfrm;
struct Transform {

    vec3 position;
    quat rotation;
    vec3 scale;

    trsfrm* parent;
};

/// @brief Function which frees and object
/// @param toFree The object to free
typedef void func_free(void* toFree);

/// @brief Structure for attaching data to a scene object
typedef struct ExternalData {

    uint ID;
    void* data;
    func_free* freeData;

} ext_data;

/// @brief Create an external data block to use with a scene object
/// @param ID The unique identifier of this external data block
/// @param data The data to package
/// @param freeData The appropriate function to free this data when done 
/// @return The block of data
static inline ext_data extDataPackage(uint ID, void* data, func_free* freeData) { return (ext_data){ID, data, freeData}; }



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////// * * *  SCENE OBJECTS  * * * ////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




/// @brief Structure representing an object in the scene
typedef struct SceneObject sc_obj;

/// @brief Function which updates a scene object every frame
/// @param self The scene object to update
typedef void func_update(sc_obj* self);

struct SceneObject {
    trsfrm transform;

    func_update* update; // SHOULD BECOME A LIST TIED WITH EXTERNAL DATA
    bool isStatic; // If this object never updates
    bool isActive; // if the object is currently active
    
    list_t externalData;
};

/// @brief Create new scene object
/// @param position The initial position of the object in world-space
/// @param rotation The initial rotation of the object in world-space
/// @param scale The initial scale of the object in world-space
/// @param parent The parent of the object's transform (May be NULL)
/// @param freeTransformData Whether to free the values pointed to by "position", "rotation" and "scale"
/// @param isStatic If the obect is static (Static objects are supposed unmoving and don't update, thus optimizations can be done on them)
/// @param update The update function of this object. Is called every frame if non-static (May be NULL)
/// @return The newly created scene object 
sc_obj* newSceneObject(
    vec3* position, quat* rotation, vec3* scale, trsfrm* parent, bool freeTransformData,
    bool isStatic, func_update* update
);

/// @brief Free a scene object
/// @param obj The object to free
void freeSceneObject(sc_obj* obj);

/// @brief Check if an object is marked static
/// @param obj The scene object to verify
/// @return If the object is marked static
bool scobjIsStatic(sc_obj* obj);

/// @brief Check if an object is active
/// @param obj The scene object
/// @return Wether the object is active
bool scobjIsActive(sc_obj* obj);
/// @brief Set an object's activeness
/// @param obj The scene object
/// @param active The value to set
void scobjSetActive(sc_obj* obj, bool active);

/// @brief Remove an external data block from a scene object
/// @param obj The object from which to remove the block
/// @param ID The unique ID of the block to remove
/// @return Whether an external data with this ID was removed
bool scobjRemoveExtData(sc_obj* obj, uint ID);

/// @brief Add an external data block to a scene object
/// @param obj The object to which the block is added
/// @param externalData The external data block to add
void scobjAddExtData(sc_obj* obj, ext_data externalData);

/// @brief Package and add an external data block to a scene object
/// @param obj The object to which the block is added
/// @param ID The unique identifier of this external data block
/// @param data The data to package
/// @param freeData The appropriate function to free this data when done 
void scobjAddExtDataP(sc_obj* obj, uint ID, void* data, func_free* freeData);

/// @brief Get an object's external data by ID. If the ID is not unique, it will get the first one.
/// @param obj The scene object
/// @param ID The ID of the data to retrieve
/// @return A pointer to the data
void* scobjGetExtData(sc_obj* obj, uint ID);

/// @brief Call a scene object's update method
/// @param obj The object to process
void scobjUpdate(sc_obj* obj);

/// @brief Get world-space position of a scene object
/// @param obj The scene object
/// @return The world-space position
vec3 scobjWorldPos(sc_obj* obj);
/// @brief Get world-space rotation of a scene object
/// @param obj The scene object
/// @return The world-space rotation
quat scobjWorldRot(sc_obj* obj);



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////// * * *  CAMERA  * * * ////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



// Unique identifier for Camera
#define EXT_ID_CAMERA 1
#define CAMERA_UNIFORM_BUFFER_BINDING_IDX 1
#define CAMERA_UNIFORM_BUFFER_BINDING_NAME "Camera"

/// @brief Camera external data structure
/// @warning Use the predefined functions to modify this object's parameters
typedef struct Camera cam;

/// @brief Create a new camera object
/// @param position The initial position of the object in world-space
/// @param rotation The initial rotation of the object in world-space
/// @param parent The parent of the object's transform (May be NULL)
/// @param freeTransformData Whether to free the values pointed to by "position" and "rotation"
/// @param FOV The Field Of View of the camera (In degrees)
/// @param nearClippingPlane The shortest visible distance
/// @param farClippingPlane The furthest visible distance
/// @param screenRatio The width-to-height ratio of the camera
/// @return The newly created camera
sc_obj* newCamera(vec3* position, quat* rotation, trsfrm* parent, bool freeTransformData, float FOV, float nearClippingPlane, float farClippingPlane, float screenRatio);

/// @brief Change the camera's Field Of View
/// @param camera The camera to modify
/// @param newFOV The desired value 
void camSetFOV(cam* camera, float newFOV);
/// @brief Get the camera's Field Of View
/// @param camera The camera from which to extract the data
/// @return The camera's Field Of View
float camGetFOV(cam* camera);

/// @brief Change the camera's Near Clipping Plane
/// @param camera The camera to modify
/// @param newNCP The desired value 
void camSetNCP(cam* camera, float newNCP);

/// @brief Change the camera's Far Clipping Plane
/// @param camera The camera to modify
/// @param newFCP The desired value 
void camSetFCP(cam* camera, float newFCP);

/// @brief Change the camera's width-to-height ratio
/// @param camera The camera to modify
/// @param newRatio The desired value 
void camSetRatio(cam* camera, float newRatio);
/// @brief Get camera near and far clipping planes, view width and view height
/// @param camera The camera from which to extract the data
/// @return Near clipping plane, far clipping plane, view width, view height (in this order)
vec4 camGetNCP_FCP_W_H(cam* camera);

/// @brief Get the source scene object of this camera external data
/// @param c The camera external data
/// @return The source scene object
sc_obj* camGetScObj(cam* c);

void camPrepareForRender(cam* c);



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////// * * *  RENDER OBJECT  * * * /////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



#define EXT_ID_RENDER_OBJ 2
#define RENDER_OBJ_UNIFORM_BUFFER_BINDING_IDX 2
#define RENDER_OBJ_UNIFORM_BUFFER_BINDING_NAME "Object"
/// @brief The different culling modes
typedef enum RenderObjectCulling {
    RENDER_CULL_NONE = 0b00000000,
    RENDER_CULL_FRONT = 0b00000001,
    RENDER_CULL_BACK = 0b00000010,
    RENDER_CULL_BOTH = 0b00000011
} render_obj_cull;

/// @brief Function just before an object is rendered
/// @param obj The object to render
typedef void func_onBeforeRender(sc_obj* obj);
/// @brief Function just after an object has been rendered
/// @param obj The object rendered
typedef void func_onAfterRender(sc_obj* obj);

/// @brief Render object external data structure
typedef struct RenderObject {
    mesh* mesh;
    array* materials;

    func_onBeforeRender* onBeforeRender;
    func_onAfterRender* onAfterRender;

    union {
        struct {
            bool render : 1;
            render_obj_cull cull : 2;
            bool castShadow : 1;
            bool transparent : 1;
        };
        uint8 flags;
    };
} render_obj;

/// @brief Add a new render object external data block
/// @param source The scene object on which this data is to be added
/// @param mesh The mesh of the object
/// @param materials The materials with which to render
/// @param render If the object should render
/// @param cull How the object should cull
/// @param castShadow If the object casts shadows
/// @param onBeforeRender Function called just before a render
/// @param onAfterRender Function called just after a render
void scobjAddRenderObjectExtData(
    sc_obj* source,
    mesh* mesh, array* materials,
    bool render, render_obj_cull cull, bool castShadow,
    func_onBeforeRender* onBeforeRender, func_onAfterRender* onAfterRender
);
/// @brief Add a new render object external data block
/// @param source The scene object on which this data is to be added
/// @param mesh The mesh of the object
/// @param material The material with which to render
/// @param render If the object should render
/// @param cull How the object should cull
/// @param castShadow If the object casts shadows
/// @param onBeforeRender Function called just before a render
/// @param onAfterRender Function called just after a render
void scobjAddRenderObjectExtData_SingleMat(
    sc_obj* source,
    mesh* mesh, material* material,
    bool render, render_obj_cull cull, bool castShadow,
    func_onBeforeRender* onBeforeRender, func_onAfterRender* onAfterRender
);

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