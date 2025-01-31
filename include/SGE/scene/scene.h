#ifndef __SGE_SCENE_SCENE_H__
#define __SGE_SCENE_SCENE_H__

#include "sc_object.h"

typedef struct Scene {
    list(sc_obj) sceneObjects;
} scene;

/// @brief Creates a new empty scene
/// @return The newly created scene
scene createScene();

/// @brief Destroy an existing scene
/// @param toDestoy The scene to destroy
void destroyScene(scene* toDestoy);

/// @brief Add a scene object to the scene
/// @param sc The scene in which to add the object
/// @return The newly created object (with default parameters)
sc_obj* sceneAddObject(scene* sc);

/// @brief Update every scene object
void sceneUpdate();

#endif
