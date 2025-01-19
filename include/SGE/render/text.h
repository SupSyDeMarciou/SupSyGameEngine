#ifndef __SGE_RENDER_TEXT_H__
#define __SGE_RENDER_TEXT_H__

#include "../constants.h"

#include "rend_env.h"
#include "shaders/shaders.h"
#include "mesh.h"
#include "../scene/sc_object.h"

typedef struct Font font;
struct Font {
    const char* path;
    uint32* cmap;
    uint16 unitsPerEm;
    
    // Unique glyph data
    struct Glyph {
        int32 x, y, dx, dy;
        uint32 advanceWidth;
        int32 nbContours;
        uint32 endOfFirstContour;
        uint32 firstPoint;
    }* glyphs;
    uint32 nbGlyphs;
    
    // All points constituting the contours
    ivec2* glyphPoints;
    uint64 nbPoints;
    
    // Index of points defining contour ends
    uint32* lastContourPoints;
    uint32 nbContours;

    ivec2 minBound; // The minimum of all bounding boxes
    ivec2 maxBound; // The maximum of all bounding boxes

    GLuint glyphDataSBO; // All of the data unique to 1 glyph
    GLuint pointsSBO; // All of the points making up the splines
    GLuint endContoursSBO; // All of indexes of the last points in a contour
    bool synched;
};
static font* CURRENT_FONT = NULL;

void fontSetDefaultPath(const char* folder);
font* fontImport(const char* path);
uint32 fontGetGlyphIdx(font* f, uint32 character);
void fontUpdateGPUData(font* f);




#define char8 char
#define char16 __CHAR16_TYPE__

typedef struct TextGlyphData {
    vec2 offset;
    uint idx;
    vec4 color;
    float size;
} text_glyph_data;
typedef enum TextAnchor {
    TEXT_ANCHOR_LEFT = 0b00000001,
    TEXT_ANCHOR_CENTER_H = 0b00000010,
    TEXT_ANCHOR_RIGHT = 0b00000011,

    TEXT_ANCHOR_TOP = 0b00000100,
    TEXT_ANCHOR_CENTER_V = 0b00001000,
    TEXT_ANCHOR_BOTTOM = 0b00001100,

    TEXT_ANCHOR_CENTER = 0b00001010
} text_anchor;
typedef struct Text {
    font* f;                // font
    mesh* m;                // quad, to be instanced

    uint8 anchor;
    
    char16* text;
    float* textSize;
    vec4* textCol;
    text_glyph_data* textData; // data for each character
    uint64 textLength;
    uint64 textCapa;

    float baseFontSize;     // base character size
    vec4 baseFontColor;     // base character color

    int16 fontSpaceH;       // character horizontal spacing
    int16 fontSpaceV;       // character vertical spacing
    vec2 boxSize;           // Max size in which to fit the text
    vec2 trueBoxSize;       // Size of the box currently containing all text

    GLuint instanceVbo;     // instance vbo
    bool synched;           // wether the data is synchronized on GPU
} text;

text* newText(font* f, char16* str, vec2 boxSize, text_anchor anchor);
text* newText_Base(font* f, char16* str, vec2 boxSize, text_anchor anchor, float baseFontSize, vec4 baseFontColor);
text* newText_Advanced(font* f, char16* str, vec2 boxSize, text_anchor anchor, float baseFontSize, vec4 baseFontColor, int16 fontSpaceH, int16 fontSpaceV);
void freeText(text* toFree);
void textMarkUnsynched(text* t);

void textSetString(text* t, char16* s);
void textSetString_Span(text* t, char16* s, uint start, uint span);
void textSetColor_Span(text* t, vec4 color, uint start, uint span);
void textSetColor_Base(text* t, vec4 color);
void textSetScale_Span(text* t, float size, uint start, uint span);
void textSetScale_Base(text* t, float size);
void textSetBoxSize(text* t, vec2 size);

void textRender(text* t, shader s, vec2 position);

vec2 textGetChar_Pos(text* t, uint idx);
float textGetChar_Scale(text* t, uint idx);



typedef struct Text3D {
    sc_obj* source;
    text* t;
    shader s;
} text3D;
DEF_EXT_ID(text3D);

void text3DRender(sc_obj* source);
text3D* scobjAttachText3D(sc_obj* source, text* t, shader s);

#endif