#include "shader.h"

// CACHE UNIFORM LOCATIONS HERE
typedef struct Private_Shader {
    GLuint gl_shader_id;
    char** fields;
} p_shader;

static char defaultLocation[1024] = "shaders";

void shaderSetDefaultPath(const char* folder) {
    sprintf(defaultLocation, folder);
}

static void getFilePath(const char* filePath, char output[1024]) {
    
    if (filePath[0] == '!') {
        sprintf(output, "%s/%s", __SUPSY_SGE_PATH, &filePath[1]);
        return;
    }
    
    int l = 0;
    while (filePath[l] != '\0' && filePath[l] != '/') l++;
    const char fileLocation[1024];
    if (filePath[l] == '/') sprintf(output, "%s", filePath); // Is path
    else sprintf(output, "%s/%s", defaultLocation, filePath); // Is only file name
}

// /*
typedef struct ShaderReader {
    char* buffer;
    uint p;
    uint capacity;
} shader_reader;
static void shaderLoad_Part(const char* path, shader_reader* sr) {

    char truePath[1024] = {0};
    getFilePath(path, truePath);

    char* shaderText = SL_readFile(truePath);
    uint l = 0;

    while (shaderText[l]) {

        if (shaderText[l] == '#') {
            if (SL_matchFactor(shaderText + l + 1, "include")) {
                l += sizeof("include");

                while (shaderText[l] && shaderText[l] != '\"') l++;
                if (!shaderText[l]) failWithError("Failed to compile \"%s\". Missing path in \'#include\' statement.", path);
                l++; // "

                uint k = 0;
                char includePath[1024] = {0};
                while (shaderText[l] && shaderText[l] != '\"') {
                    includePath[k++] = shaderText[l++];
                }
                if (!shaderText[l]) failWithError("Failed to compile \"%s\". Missing \'\"\' in \'#include\' statement.", path);
                includePath[k] = '\0';
                l++;

                // printf("Include \"%s\"\n", includePath);
                shaderLoad_Part(includePath, sr);

                continue;
            }
            else if (SL_matchFactor(shaderText + l + 1, "request")) {
                // TODO: link data to shader
                printf("request\n");
                l += sizeof("request");
                
                continue;
            }
        }

        sr->buffer[sr->p++] = shaderText[l++];
        if (sr->p >= sr->capacity - 1) sr->buffer = realloc(sr->buffer, sr->capacity *= 2);
    }
    
    free(shaderText);
}
static char* shaderLoad(const char* path) {

    shader_reader sr = (shader_reader){
        .buffer = malloc(1024), // Start
        .p = 0,
        .capacity = 1024
    };

    shaderLoad_Part(path, &sr);

    sr.buffer[sr.p] = '\0';
    // printf("Shader \"%s\":\n%s\n", path, sr.buffer);

    return sr.buffer;
}
// */

// static char* shaderLoad(const char* path) {
//     return readFile(path);
// }

static int initializeShader(const char* filePath, GLuint type) {
    char fileLocation[1024];
    getFilePath(filePath, fileLocation);

    const char* sourceCode = shaderLoad(fileLocation);

    int shaderObject = glCreateShader(type);
    glShaderSource(shaderObject, 1, &sourceCode, NULL);
    glCompileShader(shaderObject);
    free((char*)sourceCode);

    int success;
    glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &success);

    if (!success) {
        char infoLog[512] = {'\0'};
        glGetShaderInfoLog(shaderObject, 512, NULL, infoLog);
        failWithError("Failed to create vertex shader \"%s\"\nOpenGL error: %s", filePath, infoLog);
    }

    return shaderObject;
}
shader createShader(const char* vertexShaderFilePath, const char* fragmentShaderFilePath, const char* geometryShaderFilePath) {

    uint vertexShaderObject = initializeShader(vertexShaderFilePath, GL_VERTEX_SHADER);
    printf("Imported vertex shader \'%s\'\n", vertexShaderFilePath);

    uint fragmentShaderObject = initializeShader(fragmentShaderFilePath, GL_FRAGMENT_SHADER);
    printf("Imported frament shader \'%s\'\n", fragmentShaderFilePath);

    uint geometryShaderObject;
    if (geometryShaderFilePath) { 
        geometryShaderObject = initializeShader(geometryShaderFilePath, GL_GEOMETRY_SHADER);
        printf("Imported geometry shader \'%s\'\n", geometryShaderFilePath);
    }

    shader shaderProgramObject = glCreateProgram();

    glAttachShader(shaderProgramObject, vertexShaderObject);
    glAttachShader(shaderProgramObject, fragmentShaderObject);
    if (geometryShaderFilePath) glAttachShader(shaderProgramObject, geometryShaderObject);
    glLinkProgram(shaderProgramObject);

    int success;
    glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &success);

    if (!success) {
        char infoLog[512] = {'\0'};
        glGetProgramInfoLog(shaderProgramObject, 512, NULL, infoLog);
        failWithError("Failed to create program\nOpenGL error: %s", infoLog);
    }

    glDeleteShader(vertexShaderObject);
    glDeleteShader(fragmentShaderObject);
    if (geometryShaderFilePath) glDeleteShader(geometryShaderObject);

    return shaderProgramObject;
}

void freeShader(shader s) {
    glDeleteProgram(s);
}

void shaderAttachUniformBufferBP(shader s, const sbu_bp* bindingPoint) {
    GLuint unifromBufferIdx = glGetUniformBlockIndex(s, bindingPoint->name);
    if (unifromBufferIdx == GL_INVALID_INDEX) {
        throwWarning("Uniform buffer \"%s\" not found in shader", bindingPoint->name);
        return;
    } 
    glUniformBlockBinding(s, unifromBufferIdx, bindingPoint->bindingIDX);
}
void shaderAttachShaderStorageBP(shader s, const sbs_bp* bindingPoint) {
    GLuint shaderStorageIdx = glGetProgramResourceIndex(s, GL_SHADER_STORAGE_BLOCK, bindingPoint->name);
    if (shaderStorageIdx == GL_INVALID_INDEX) {
        throwWarning("Storage buffer \"%s\" not found in shader", bindingPoint->name);
        return;
    } 
    glShaderStorageBlockBinding(s, shaderStorageIdx, bindingPoint->bindingIDX);
}
void shaderAttachShaderStorage_noBP(shader s, const char* name, uint bindingIDX) {
    GLuint shaderStorageIdx = glGetProgramResourceIndex(s, GL_SHADER_STORAGE_BLOCK, name);
    if (shaderStorageIdx == GL_INVALID_INDEX) {
        throwWarning("Storage buffer \"%s\" not found in shader", name);
        return;
    } 
    glShaderStorageBlockBinding(s, shaderStorageIdx, bindingIDX);
}

#include "../../scene/sc_object.h"
static const char defaultVertexShaderPath[] = "!rendering/shaders/vertex.vs";
shader createSurfaceShader(const char* fileName) {
    shader s = createShader(defaultVertexShaderPath, fileName, NULL);
    shaderAttachUniformBufferBP(s, SGE_SBU_BP_Environment);
    shaderAttachUniformBufferBP(s, SGE_SBU_BP_Camera);
    shaderAttachUniformBufferBP(s, SGE_SBU_BP_Object);
    printf("Create surface shader\n");
    return s;
}

static shader inUse = 0;
void shaderUse(shader s) {
    if (s != inUse) glUseProgram(s);
}

void shaderSetBool(shader s, const char* fieldName, bool value) {
    shaderUse(s);
    glUniform1i(glGetUniformLocation(s, fieldName), value);
}

void shaderSetInt(shader s, const char* fieldName, int value) {
    shaderUse(s);
    glUniform1i(glGetUniformLocation(s, fieldName), value);
}

void shaderSetInt2(shader s, const char* fieldName, int x, int y) {
    shaderUse(s);
    glUniform2i(glGetUniformLocation(s, fieldName), x, y);
}

void shaderSetInt3(shader s, const char* fieldName, int x, int y, int z) {
    shaderUse(s);
    glUniform3i(glGetUniformLocation(s, fieldName), x, y, z);
}

void shaderSetInt4(shader s, const char* fieldName, int x, int y, int z, int w) {
    shaderUse(s);
    glUniform4i(glGetUniformLocation(s, fieldName), x, y, z, w);
}

void shaderSetUint(shader s, const char* fieldName, uint value) {
    shaderUse(s);
    glUniform1ui(glGetUniformLocation(s, fieldName), value);
}

void shaderSetUint2(shader s, const char* fieldName, uint x, uint y) {
    shaderUse(s);
    glUniform2ui(glGetUniformLocation(s, fieldName), x, y);
}

void shaderSetUint3(shader s, const char* fieldName, uint x, uint y, uint z) {
    shaderUse(s);
    glUniform3ui(glGetUniformLocation(s, fieldName), x, y, z);
}

void shaderSetUint4(shader s, const char* fieldName, uint x, uint y, uint z, uint w) {
    shaderUse(s);
    glUniform4ui(glGetUniformLocation(s, fieldName), x, y, z, w);
}

void shaderSetFloat(shader s, const char* fieldName, float value) {
    shaderUse(s);
    glUniform1f(glGetUniformLocation(s, fieldName), value); 
}

void shaderSetFloat2(shader s, const char* fieldName, float x, float y) {
    shaderUse(s);
    glUniform2f(glGetUniformLocation(s, fieldName), x, y); 
}

void shaderSetFloat3(shader s, const char* fieldName, float x, float y, float z) {
    shaderUse(s);
    glUniform3f(glGetUniformLocation(s, fieldName), x, y, z); 
}

void shaderSetFloat4(shader s, const char* fieldName, float x, float y, float z, float w) {
    shaderUse(s);
    glUniform4f(glGetUniformLocation(s, fieldName), x, y, z, w);
}

void shaderSetVec2(shader s, const char* fieldName, const vec2* v) {
    shaderUse(s);
    glUniform2f(glGetUniformLocation(s, fieldName), v->x, v->y); 
}

void shaderSetVec3(shader s, const char* fieldName, const vec3* v) {
    shaderUse(s);
    glUniform3f(glGetUniformLocation(s, fieldName), v->x, v->y, v->z); 
}

void shaderSetVec4(shader s, const char* fieldName, const vec4* v) {
    shaderUse(s);
    glUniform4f(glGetUniformLocation(s, fieldName), v->x, v->y, v->z, v->w); 
}

void shaderSetQuat(shader s, const char* fieldName, const quat* q) {
    shaderUse(s);
    glUniform4f(glGetUniformLocation(s, fieldName), q->x, q->y, q->z, q->w); 
}

void shaderSetMat2x2(shader s, const char* fieldName, const mat2x2* m, bool transpose) {
    shaderUse(s);
    glUniformMatrix2fv(glGetUniformLocation(s, fieldName), 1, transpose, &(m->m[0])); 
}

void shaderSetMat3x3(shader s, const char* fieldName, const mat3x3* m, bool transpose) {
    shaderUse(s);
    glUniformMatrix3fv(glGetUniformLocation(s, fieldName), 1, transpose, &(m->m[0])); 
}

void shaderSetMat4x4(shader s, const char* fieldName, const mat4x4* m, bool transpose) {
    shaderUse(s);
    glUniformMatrix4fv(glGetUniformLocation(s, fieldName), 1, transpose, &(m->m[0]));
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////// * * *  COMPUTE SHADER  * * * ////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////// COMPUTE SHADER
comp_shader createComputeShader(const char* filePath) {

    uint computeShaderObject = initializeShader(filePath, GL_COMPUTE_SHADER);
    printf("Imported compute shader \'%s\'\n", filePath);

    comp_shader shaderProgramObject = glCreateProgram();

    glAttachShader(shaderProgramObject, computeShaderObject);
    glLinkProgram(shaderProgramObject);

    int success;
    glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &success);

    if (!success) {
        char infoLog[512] = {'\0'};
        glGetProgramInfoLog(shaderProgramObject, 512, NULL, infoLog);
        failWithError("Failed to create program\nOpenGL error: %s", infoLog);
    }
    glDeleteShader(computeShaderObject);

    return shaderProgramObject;
}

void compShaderDispatch(comp_shader s, uint x, uint y, uint z) {
    shaderUse(s);
    glDispatchCompute(x, y, z);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}