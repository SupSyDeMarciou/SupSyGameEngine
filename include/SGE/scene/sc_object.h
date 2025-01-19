#ifndef __SGE_SCENE_SC_OBJECT_H__
#define __SGE_SCENE_SC_OBJECT_H__

#include <stdarg.h>
#include "../constants.h"

#include "../render/mesh.h"
#include "../render/material.h"


#define EXT_ID(type) __EXT_ID_##type
#define DEF_EXT_ID(type) extern volatile uint EXT_ID(type);
#define DEF_EXT_ID_C(type) volatile uint EXT_ID(type) = 0;
/// @brief Register an external data block fully BY ID
/// @param id must be "&EXT_ID(type)"
/// @param name Should be the exact name of the structure to register
/// @param destroy The function which destroys the block. MUST NOT FREE THE BLOCK
/// @param defaultValue The defaultValue for this data block. MAY BE NULL
/// @param required The requied external data blocks to have on the object before attaching this one
/// @return Wether this data has just been registered
bool registerExtData_ID_Full(volatile uint* id, const char* name, func_destroy* f, void* defaultValue, array_void required);
/// @brief Register an external data block BY ID
/// @param id must be "&EXT_ID(type)"
/// @param name Should be the exact name of the structure to register
/// @param destroy The function which destroys the block. MUST NOT FREE THE BLOCK
/// @return Wether this data has just been registered
bool registerExtData_ID(volatile uint* id, const char* name, func_destroy* f);
/// @brief Register an external data block
/// @param data The type of the data to register
/// @param destroy The function which destroys the block. MUST NOT FREE THE BLOCK
#define registerExtData(data, f) do { if (!registerExtData_ID(&EXT_ID(data), #data, f)) SGE_throwError("External data \""#data"\" was already registered!"); } while (false)


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////// * * *  SCENE OBJECTS  * * * ////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/// @brief Structure representing an object's spacial coordinates
typedef struct Transform trsfrm;
struct Transform {

    vec3 position;
    quat rotation;
    vec3 scale;

    trsfrm* parent;
};

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
    
    list externalData;
};

/// @brief Create new scene object
/// @param position The initial position of the object in world-space
/// @param rotation The initial rotation of the object in world-space
/// @param scale The initial scale of the object in world-space
/// @param parent The parent of the object's transform (May be NULL)
/// @param isStatic If the obect is static (Static objects are supposed unmoving and don't update, thus optimizations can be done on them)
/// @param update The update function of this object. Is called every frame if non-static (May be NULL)
/// @return The newly created scene object 
sc_obj* newSceneObject(vec3 position, quat rotation, vec3 scale, trsfrm* parent, bool isStatic, func_update* update);

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

/// @brief Remove an external data block from a scene object by ID
/// @param obj The object from which to remove the block
/// @param ID The ID of the data block
/// @return Whether an external data with this ID was removed
bool scobjRemoveExtData_ID(sc_obj* obj, uint ID);
/// @brief Remove an external data block from a scene object by type
/// @param obj The object from which to remove the block
/// @param type The type of the data block
/// @return Whether an external data with this type was removed
#define scobjRemoveExtData(obj, type) scobjRemoveExtData_ID(obj, EXT_ID(type))

/// @brief Package and add an external data block to a scene object by ID
/// @param obj The object to which the block is added
/// @param ID The ID of the data block
/// @param data The data to package
/// @return Wether the data has been attached
bool scobjAttachExtData_ID(sc_obj* obj, uint ID, void* data);
/// @brief Package and add an external data block to a scene object by type
/// @param obj The object to which the block is added
/// @param type The type of the data block
/// @param data The data to package
#define scobjAttachExtData(obj, type, data) if (!scobjAttachExtData_ID(obj, EXT_ID(type), data)) SGE_throwError("Type \""#type"\" was not registered as an external data block");

/// @brief Get an object's external data by ID.
/// @param obj The scene object
/// @param ID The ID of the data block
/// @return A pointer to the first matching data or NULL if none was found
void* scobjGetExtData_ID(sc_obj* obj, uint ID);
/// @brief Get an object's external data by type.
/// @param obj The scene object
/// @param type The type of the data block
/// @return A pointer to the first matching data or NULL if none was found
#define scobjGetExtData(obj, type) ((type*)scobjGetExtData_ID(obj, EXT_ID(type)))

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
/// @brief Get world-space scale of a scene object
/// @param obj The scene object
/// @return The world-space scale
vec3 scobjWorldScale(sc_obj* obj);



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////// * * *  CAMERA  * * * ////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/// @brief Camera external data structure
/// @warning Use the predefined functions to modify this object's parameters
typedef struct Camera cam;
DEF_EXT_ID(cam)

/// @brief Create a new camera object
/// @param FOV The Field Of View of the camera
/// @param nearClippingPlane The shortest visible distance
/// @param farClippingPlane The furthest visible distance
/// @param renderRatio The width-to-height ratio of the camera
/// @return The newly created camera
cam* scobjAttachCamera(sc_obj* source, float FOV, float nearClippingPlane, float farClippingPlane, float renderRatio);

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

/// @brief Convert position in screen-space coordinates into world-space coordinates
/// @param screenPos The position in screen-space
/// @param distance The distance from the camera of the converted coordinate
/// @return The converted position in world-space
/// @note The convertion depends on the current rendering camera
vec3 camScreenToWorld(vec2 screenPos, float distance);

#endif