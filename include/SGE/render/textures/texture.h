#ifndef __SGE_RENDERING_TEXTURES_TEXTURE
#define __SGE_RENDERING_TEXTURES_TEXTURE

#include "../../SGEstructures.h"
#include "../shaders/shader.h"

/// @brief Color components in texture (R, G, B and A + linear?)
typedef enum TextureColorComponents {
    TEX_COL_R = GL_RED,
    TEX_COL_RG = GL_RG,
    TEX_COL_RGB = GL_RGB,
    TEX_COL_RGBA = GL_RGBA,
    TEX_COL_sRGB = GL_SRGB,
    TEX_COL_sRGBA = GL_SRGB_ALPHA
} tex_col_comp;

/// @brief Bit depth of color
typedef enum TextureBitDepth {
    TEX_BIT_DEPTH_2 = 2,
    TEX_BIT_DEPTH_4 = 4,
    TEX_BIT_DEPTH_5 = 5,
    TEX_BIT_DEPTH_8 = 8,
    TEX_BIT_DEPTH_10 = 10,
    TEX_BIT_DEPTH_12 = 12,
    TEX_BIT_DEPTH_16 = 16,
    TEX_BIT_DEPTH_32 = 32
} tex_bit_depth;

/// @brief Format of color data
typedef enum TextureDataFormat {
    TEX_FORM_UNSIGNED_NORM,
    TEX_FORM_SIGNED_NORM,
    TEX_FORM_UNSIGNED_INT,
    TEX_FORM_SIGNED_INT,
    TEX_FORM_FLOAT
} tex_dat_form;

/// @brief Format of a texture
typedef struct TextureFormat tex_form;
struct TextureFormat {
    tex_col_comp format;
    uint internalFormat;
};

/// @brief Formats that can't be built from combining tex_color_[]
typedef enum OtherTextureFormats {
    TEX_O_R3G3B2 = GL_R3_G3_B2,
    TEX_O_RGB5A1 = GL_RGB5_A1,
    TEX_O_RGB10A2 = GL_RGB10_A2,
    TEX_O_RGB10A2UI = GL_RGB10_A2UI,
    TEX_O_R11G11B10F = GL_R11F_G11F_B10F,
    TEX_O_R5G6B5 = GL_RGB565
} tex_other_formats;

/// @brief Texture wrapping modes
typedef enum TextureWrapMode {
    TEX_WRAP_REPEAT = GL_REPEAT, // If the texture repeats outside of bounds
    TEX_WRAP_MIRRORED = GL_MIRRORED_REPEAT, // If the texture is mirrored outside of bounds
    TEX_WRAP_CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE, // If the color is extended oustide of bounds
    TEX_WRAP_CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER // If the color is user set outside of bounds
} tex_wrap_mode;

/// @brief Texture filtering modes
typedef enum TextureFilterMode {
    TEX_FILTER_NEAREST = GL_NEAREST, // Closest neighbour filter mode
    TEX_FILTER_BILINEAR = GL_LINEAR, // Bilinear sampleling filter mode
} tex_filter_mode;

/// @brief Texture MipMap interpolation modes
typedef enum TextureMipMap {
    TEX_MIPMAP_NONE = 0, // No interpolation between mipmaps
    TEX_MIPMAP_NEAREST = 0x100, // Nearest neighbour interpolation
    TEX_MIPMAP_BILINEAR = 0x101 // Linear interpolation between mipmaps
} tex_mipmap;

/// @brief Generate a valid texture format
/// @param component The channels to be present on this texture
/// @param bitDepth The number of bits to allocate to each channel
/// @param format The format of the data
/// @return A valid texture format
tex_form tex2DGenFormat(tex_col_comp component, tex_bit_depth bitDepth, tex_dat_form format);
/// @brief Generate a valid texture format from exotic format type
/// @param otherFormat Exotic format type
/// @return A valid texture format
tex_form tex2DGenOtherFormat(tex_other_formats otherFormat);


/// @brief 2D Texture structure
typedef struct Texture2D texture2D;

/// @brief Create new 2D texture
/// @param size The size of the texture
/// @param format The format of the texture (Use "tex2DGenFormat" or "tex2DGenOtherFormat" as input)
/// @return The newly created 2D texture
texture2D* newTexture2D(uvec2 size, tex_form format);
/// @brief Convert a 2D image into a 2D texture
/// @param image The source image
/// @return The newly created 2D texture
texture2D* newTexture2D_fromImage2D(image2D image);
/// @brief Load a 2D image into a 2D texture
/// @param path The path to the image
/// @param extension The extension of the image
/// @return The newly created 2D texture
texture2D* newTexture2D_load(const char* path, img_ext extension);
/// @brief Free a 2D texture
/// @param toFree The texture to free
void freeTexture2D(texture2D* toFree);

/// @brief Set a 2D texture's wrapping mode
/// @param texture The 2D texture
/// @param wrapS The wrapping mode in S direction
/// @param wrapT The wrapping mode in T direction
void tex2DSetWrap(texture2D* texture, tex_wrap_mode wrapS, tex_wrap_mode wrapT);
/// @brief Set a 2D texture's filter mode
/// @param texture The 2D texture
/// @param magnifyFilter The filter mode when magnifying (going higher res)
/// @param minifyFilter The filter mode when minifying (going lower res)
void tex2DSetFilter(texture2D* texture, tex_filter_mode magnifyFilter, tex_filter_mode minifyFilter);
/// @brief Set a 2D texture's filter mode
/// @param texture The 2D texture
/// @param magnifyMipMap The filter mode when magnifying a mip-map (going higher res)
/// @param minifyMipMap The filter mode when minifying a mip-map (going lower res)
/// @param mipmapLevel The number of mip-maps to create
void tex2DSetMipMap(texture2D* texture, tex_mipmap magnifyMipMap, tex_mipmap minifyMipMap, uint mipmapLevel);
/// @brief Override a 2D texture's format
/// @param texture The 2D texture
/// @param format The new format
void tex2DOverrideFormat(texture2D* texture, tex_form format);
/// @brief Get a 2D texture's format
/// @param texture The 2D texture
/// @return The texture's format
tex_form tex2DGetFormat(const texture2D* texture);
/// @brief Apply the CPU-side changes on the GPU
/// @param texture The texture
/// @note This function should be called after all the changes to the target texture have been registered with the "tex2DSet..." functions
void tex2DApplyChanges(texture2D* texture);
/// @brief Override a 2D texture's size
/// @param texture The 2D texture
/// @param newSize The new size
void tex2DOverrideSize(texture2D* texture, uvec2 newSize);
/// @brief Get a 2D texture's size
/// @param texture The 2D texture
/// @return The texture's size
uvec2 tex2DGetSize(const texture2D* tex);

/// @brief Get the OpenGL ID of a 2D texture
/// @param tex The 2D texture
/// @return The requested ID
/// @note This is the object identifier OpenGL returns when creating objects on the GPU
GLuint tex2DGetGlID(const texture2D* tex);

/// @brief Set a shader image texture
/// @param s Shader ID
/// @param fieldName Field name of the variable to set
/// @param textureID The texture's position in shader (relative to first texture used)
/// @param texture The desired texture
void shaderSetTexture2D(shader s, const char* fieldName, uint textureID, const texture2D* texture);

/// @brief Copy a texture into another
/// @param source The texture to copy from
/// @param dest The texture to copy to
void tex2DCopy(texture2D* source, texture2D* dest);

/// @brief Convert a texture to an image
/// @param texture The texture to convert
/// @return The generated image
image2D tex2DToImage2D(const texture2D* texture);

#endif