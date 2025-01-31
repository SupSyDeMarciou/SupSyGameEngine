#ifndef __SGE_RENDER_MESH_H__
#define __SGE_RENDER_MESH_H__

#include "shaders/shaders.h"
#include "material.h"


/// @brief Unit of data sent to the GPU
typedef struct Vertex {
    vec3 position;
    vec3 color;
    vec3 normal;
    vec2 uv;
} vertex;
SL_DEFINE_ARRAY(vertex);

/// @brief Triplet of vertices (by index)
typedef struct Triangle {
    uint a, b, c;
} triangle;
SL_DEFINE_ARRAY(triangle);

typedef struct MaterialGroup {
    uint triangle, material;
} mat_group;
SL_DEFINE_ARRAY(mat_group);

/// @brief 3D Mesh to be rendered
typedef struct Mesh mesh;
struct Mesh {
    
    vertex* vertices;
    uint nbVerts;
    triangle* triangles;
    uint nbTris;
    mat_group* materialGroups;
    uint nbMaterialGroups;

    vec3 boundingBoxLower; // Smallest (negtive) corner of the bounding box in world-space
    vec3 boundingBoxHigher; // Largest (positive) corner of the bounding box in world-space
};

/// @brief Recalculate the bounding boc of a mesh
/// @param m The mesh to modify
void meshRecalculateBoundingBox(mesh* m);
/// @brief Recalculate the normals of a mesh
/// @param m The mesh to modify
void meshRecalculateNormals(mesh* m);
/// @brief Apply the CPU-side changes on the GPU
/// @param m The mesh to modify
void meshApplyChanges(mesh* m);

// Create a new mesh with associated GPU registers
mesh* newMesh(uint nbVertices, vertex* vertices, uint nbTris, triangle* triangles, uint nbMaterialGroups, mat_group* materialGroups);
/// @brief Free a mesh
/// @param toFree The mesh to free
void freeMesh(mesh* toFree);

/// @brief Notify the SGE of changes in the mesh data
/// @param m The modified mesh
void meshMarkUnsynced(mesh* m);

/// @brief Duplicate a mesh
/// @param m The mesh to clone
/// @return The newly created clone
mesh* meshClone(mesh* m);

/// @brief Transform a mesh by a translation, rotation and scale
/// @param m The mesh
/// @param translation Translation to apply
/// @param rotation Rotation to apply
/// @param scale Scale to apply
/// @return The input mesh
mesh* meshTransform(mesh* m, vec3 translation, quat rotation, vec3 scale);
/// @brief Transform a mesh by a transform matrix
/// @param m The mesh
/// @param transformMat The transformation matrix to apply
/// @return The input mesh
mesh* meshTransform_mat4x4(mesh* m, mat4x4* transformMat);

/// @brief Merge two meshes into one
/// @param a The first mesh
/// @param b The second mesh
/// @return The newly created merged mesh
mesh* meshMerge(mesh* a, mesh* b);

// Merge mesh a and b into a new mesh with respective transforms applied
// (i) Mesh a and b are not modified
// (i) You can set an unwanted transform to NULL
// (i) Set freeTransforms to TRUE to automaticaly free all transforms
mesh* meshMerge_Transform(mesh* a, vec3 positionA, quat rotationA, vec3 scaleA, mesh* b, vec3 positionB, quat rotationB, vec3 scaleB);

/// @brief Render a mesh to the currently bound frame buffer
/// @param m The mesh to render
/// @param materials The materials associated to this mesh
void meshRender(mesh* m, array(material, p) materials);
/// @brief Render a mesh to the currently bound frame buffer
/// @param m The mesh to render
/// @param s The shader to use for the whole mesh
void meshRender_Simple(mesh* m, shader s);
/// @brief Render a mesh to the currently bound frame buffer multiple times in one render call
/// @param m The mesh to render
/// @param s The shader to use for the whole mesh
/// @param count The number of times to render the mesh
void meshRender_Instanced(mesh* m, shader s, uint count);

void meshAddVertexAttribute_Int(mesh* m, GLuint vbo, uint attributePointer, uint nbFields, GLuint type, size_t elemSize, size_t stride, uint attributeDivisor);
void meshAddVertexAttribute_Float(mesh* m, GLuint vbo, uint attributePointer, uint nbFields, bool normalized, GLuint type, size_t elemSize, size_t stride, uint attributeDivisor);
void meshAddVertexAttribute_Double(mesh* m, GLuint vbo, uint attributePointer, uint nbFields, size_t elemSize, size_t stride, uint attributeDivisor);

/// @brief Replace the vertices of a mesh
/// @param m The mesh
/// @param newVertices The new vertices 
/// @param newVertexCount The new number of vertices
void meshReplaceVertices(mesh* m, vertex* newVertices, uint newVertexCount);
/// @brief Replace the triangles of a mesh
/// @param m The mesh
/// @param newVertices The new triangles 
/// @param newVertexCount The new number of triangles
void meshReplaceTriangles(mesh* m, triangle* newTriangles, uint newTriangleCount);

/// @brief Import a mesh
/// @param path The path to the mesh
/// @return The imported mesh
mesh* meshImport(const char* path);


///////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// PRE-MADE MESHES ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

/// @brief Mesh of a quad of size 1
mesh* meshUnitQuad();

/// @brief Mesh of a grid of x by y subdivisions
/// @param x The number of divisions in x (Must be greater than 1)
/// @param y The number of divisions in y (Must be greater than 1)
mesh* meshUnitGrid(uint x, uint y);

/// @brief Mesh of a unit cube (side length 1)
mesh* meshUnitCube();

/// @brief Mesh of a cube of side length 1 with smooth normals
mesh* meshUnitCubeSmooth();

/// @brief Mesh of UV-Sphere of diameter 1
/// @param nbLatitudes Number of rings around the sphere
/// @param nbLongitudes Number of sections of the sphere
mesh* meshUnitUVShpere(uint nbLatitudes, uint nbLongitudes);

/// @brief Mesh 
/// @param nbSides Number of sides of the cylinder
/// @param radius Radius of the cylinder
/// @param height Height of the cylinder
mesh* meshCylinder(uint nbSides, float radius, float height);

#endif