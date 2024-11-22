// Object
layout (std140) uniform Object {
    vec3 u_ObjPosition;
    mat3 u_ObjRotMat;
    vec3 u_ObjScale;
    mat3 u_ObjTransformNormalMat;
    mat4 u_ObjTransformMat;
};