# SUPSY GAME ENGINE
The SupSyGameEngine (SGE) is a custom game engine made using **GLFW3**. It depends on the *SupSyLibs* (https://github.com/SupSyDeMarciou/SupSyLibrairies).

## /!\ IMPORTANT /!\
The SGE is a work in progress, therefore it has a few quirks and is not as polished as I want it to be. Some functions don't yet have a full description and some importing formats are currently either only partially supported or not supported at all (even if their name appears in the code).
Also, text rendering is not yet implemented which is very limiting, but I am working on that and am very close to incorporating it into the librairy (I just have a few rogue pixels to catch).

## Overview
The main naming conventions have been established in the README.md of the *SupSyLibrairies* GitHub repo, you can read on them through the link above.

The heart of an application built using the SGE is... well... the `application`. It has to be initialized before any other SGE feature ***AND*** GLFW3 / OpenGL feature as it creates the window and, thus, OpenGL context to which the application is rendered. The application instance is unique and can be accessed through the global variable `APP`, from which the GLFW3 window can be accessed, as well as the *renderEnvironment* and the *scene*.

____

The *renderEnvironment* of type `rend_env` is the main way the SGE interacts with the OpenGL context when doing standard 3D game raster graphics (which is all it can currently do by default). Functions acting on it are prefixed with `RE`.

The SGE also introduces a bunch of *objects* and corresponding *methods* as a way to incorporate the OpenGL routines more seamlessly within the SGE. These include:
- `shader`, `post_shader`, `comp_shader`: just a GLuint matching the OpenGL object ID of the program at the moment, but will be replaced in the future to optimise uniform writting and keep track of bindings more efficiently.
    - `post_shader` stands for "post process shader" and is a shader program which uses the `!/rendering/screen.vs` vertex shader and has no default bindings.
    - `comp_shader` stands for "compute shader" and is exactly that.

- `texture2D`: A 2D texture which has a width, height and sampling instructions (like wrapping and mip-mapping).
- `frame_buffer`: A frame buffer which can hold multiple `texture2D` objects of the same size as well a *depth-stencil* texture.

All of these have multiple functions which operate on them and usually translate to a corresponding OpenGL routine.

Other structures tied to rendering include:
- `mesh`: The representation of a 3D model, currently supporting vertex position, normal, color and UV as well as multiple materials.
- `material`: A way to encapsulate a shader and the uniform values into a single structure which makes it easier to reuse the same settings. For example when rendering the same two objects each frame which have a different color, we can just attach a different material to each instead of manualy changing the color between the render calls.
- `light`: More specific than the rest, it has a pretty explicit name. All the lights in the scene are stored in a uniform buffer which can be binded to shaders and thus used to produce more complex lighting. **(i) lights are** ***external data block*****s, which are introduced later on**

____

The *scene* is where all the *sceneObjects* of type `sc_obj` live. It is just a container for these objects and a way to call their `update` function from a single place.

*Scene objects*, on the other hand, are way very versatile and work with *external data blocks*, generally referenced in code with `ext_data` or `extData`. External data blocks can be created by the users of this librairy and are used to add custom properties to a scene object. They are packaged into an `ext_data` structure allongside a pointer to a destructor for the data, as well as a unique identifier (for now it is up to the user to make sure their IDs don't overlap with those of the SGE, which will be change with a registering system later on) to be able get acces to the data.
SGE already defines a few external data blocks:
- `trsfrm`: of ID `0`, not technically an external data block as it cannot be removed and exists for every scene object, but can be accessed with this ID nontheless
- `cam`: of ID `1`, contains all the necessary data for proper 3D rendering, as well as *methods*.
- `render_obj`: of ID `2`, is used as an interface for attaching a *mesh* and *materials* to a scene object.
- `light`: of ID `3`, was mentionned previously.

Some other external data blocks are defined within the `builtin` librairy, inside of the SGE.
