#ifndef __SGE_SCENE_SCENE
#define __SGE_SCENE_SCENE

#include "../SGEstructures.h"
#include "sc_object.h"

/// @brief Creates a new empty scene
/// @return The newly created scene
scene* createScene();

/// @brief Destroy an existing scene
/// @param toDestoy The scene to destroy
void destroyScene(scene* toDestoy);

/// @brief Add a scene object to the scene
/// @param sc The scene in which to add the object
/// @param obj The object to add
void sceneAddObject(scene* sc, sc_obj* obj);

/// @brief Update every scene object
/// @param sc The scene to update
void sceneUpdate(scene* sc);

#endif
