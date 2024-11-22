struct GlyphData {
    ivec4 coords;
    uint advanceWidth;
    int nbContours;
    uint endOfFirstContour;
    uint firstPoint;
};

layout (std430) buffer FontGlyphs {
    GlyphData b_GlyphData[];
};
layout (std430) buffer FontPoints {
    ivec2 b_GlyphPoints[];
};
layout (std430) buffer FontContourEnds {
    uint b_FontContourEnds[];
};

uniform uint u_EM;
uniform vec2 u_Position;



struct Spline { ivec2 p0, p1, p2; };
float splineEval_x(float t, Spline sp) { return ((sp.p2.x - 2 * sp.p1.x + sp.p0.x) * t + 2 * (sp.p1.x - sp.p0.x)) * t + sp.p0.x; }
float dsplineEval_y(float t, Spline sp) { return 2 * ((sp.p2.y - 2 * sp.p1.y + sp.p0.y) * t + (sp.p1.y - sp.p0.y)); }

bool within(float t, float m, float M) { return t >= m && t <= M; }
bool within01(float t) { return t >= 0.0 && t <= 1.0; }

/*
 *                         Categories are as follows:
 *
 *    .----------------------------------+-------------------------------------.
 *    |                                  |                                     |
 *    |     p1           ____x p2        |        p1           ____x p0        |
 *    |     x     ,--''''                |        x     ,--''''                |
 *    |         /'        __,            |            /'          ./           |
 *    |        /'         ./ \           |           /'         ./             |
 *    |       |'        ./               |          |'      . ./               |
 *    |      |         /                 |         |        |/                 |
 *    |      x p0                        |         x p2     ''''               |
 *    |                                  |                                     |
 *    |    CAT 0 : up all the way        |      CAT 1 : down all the way       |
 *    |        y0 <= y1 <= y2            |           y2 <= y1 <= y0            |
 *    |----------------------------------+-------------------------------------|
 *    |     p1                           |      p0 x                           |
 *    |     x     ,--'''--__             |         |     |                     |
 *    |         /'          ''- x p2     |          |.    |         __         |
 *    |        /'   /\   ''--_\          |           \.   \/   ..--''/         |
 *    |       |'    |        ''`         |            \.          __- x p2     |
 *    |      |     |                     |        x     '--...--''             |
 *    |      x p0                        |        p1                           |
 *    |                                  |                                     |
 *    |     CAT 2 : up then down         |      CAT 3 : down then up           |
 *    |       max(y0, y2) < y1           |        min(y0, y2) < y1             |
 *    |                                  |                                     |
 *    '----------------------------------+-------------------------------------'
 *
 *    For practical purposes, CAT -1 is defined as y0 = y1 = y2. It has no purpose when considering filling bezier contours and is completely ignored when encountered.
 *
*/



#define INTER_SP_MISS 0 // If an intersection missed the spline / cut it twice
#define INTER_SP_DIR(r) int(sign(dsplineEval_y(r, sp))) // Returns the direction in which the spline is pointing at on the calculated interpolation value "r"
// Get the algebraic number of intersections with the spline (based of lastN2)
int intersectSpline(vec2 pos, Spline sp, int category, int lastN2) {

    bool c1p = pos.x <= sp.p0.x && pos.y == sp.p0.y;
    bool c2p = pos.x <= sp.p2.x && pos.y == sp.p2.y;

    // If dealing with category < 2 curves, fast check for p0 and p2 intersections
    if (category < 2) {
        int n = category == 0 ? 1 : -1;
        if (c1p) return lastN2 == n ? 0 : n;
        if (c2p) return n;
    }

    float a = sp.p2.y - 2 * sp.p1.y + sp.p0.y;
    float hb = sp.p1.y - sp.p0.y;
    float c = sp.p0.y - pos.y;
    
    if (a == 0.0) {
        if (hb == 0.0) return INTER_SP_MISS; // This should not ever happen as it requires y0 = y1 = y2, which is dealt with before ever entering the body of this function

        // At this point, the line is an interpolation between two points, therfore it has category 0 or 1, so it only goes up or down
        float r = -c / hb * 0.5;
        if (within01(r) && splineEval_x(r, sp) >= pos.x) return category == 0 ? 1 : -1;
        return INTER_SP_MISS;
    }

    float qdet = hb * hb - a * c;
    if (qdet <= 0.0) return INTER_SP_MISS; // Outside of the bounds of the bezier curve or on the peak of the parabola

    float hd = sqrt(qdet);
    float den = 1.0 / a;

    float r1 = (-hd - hb) * den;
    float r2 = (hd - hb) * den;

    // Make sure r1 is the solution closer to 0.0 and r2 the one closer to 1.0, so that if pos.y = sp.p0.y or pos.y = sp.p2.y, we can exclude the closest one
    if (r1 > r2) {
        float t = r1;
        r1 = r2;
        r2 = t;
    }

    bool c1 = within01(r1) && splineEval_x(r1, sp) >= pos.x;
    bool c2 = within01(r2) && splineEval_x(r2, sp) >= pos.x;

    // Hit poles
    int n1 = category == 0 || category == 2 ? 1 : -1;
    int n2 = category == 0 || category == 3 ? 1 : -1;
    if (c1p) {
        if (c2p) return lastN2 == n1 ? n2 : INTER_SP_MISS; // If hitting p2, use category
        if (c2) return lastN2 == n1 ? INTER_SP_DIR(r2) : INTER_SP_MISS; // Else if still hitting other point, calculate direction
        return lastN2 != n1 ? n1 : INTER_SP_MISS;
    }
    if (c2p) return c1 ? INTER_SP_MISS : n2; // If hit p2, condider if hit other point before (which cannot be p0)

    // Hit body
    if (c1 && c2) return INTER_SP_MISS;
    if (c1) return INTER_SP_DIR(r1);
    if (c2) return INTER_SP_DIR(r2);
    return INTER_SP_MISS;
}

// Return maxY, minY and Category of the spline
ivec3 splineCategory(Spline sp) {
    int maxY = max(sp.p0.y, sp.p2.y);
    int minY = min(sp.p0.y, sp.p2.y);

    if (maxY == minY && sp.p1.y == minY) return ivec3(maxY, minY, -1);
    if (sp.p1.y >= minY && sp.p1.y <= maxY) return ivec3(maxY, minY, sp.p0.y < sp.p2.y ? 0 : 1);
    return sp.p1.y > maxY ? ivec3(sp.p1.y, minY, 2) : ivec3(maxY, sp.p1.y, 3);
}
// Get the algebraic number of intersections with the glyph
int hitGlyph(vec2 pos, GlyphData data) {

    int nbIntersections = 0;
    uint endOfContourPtr = data.endOfFirstContour;
    uint start = data.firstPoint;
    
    for (int i = 0; i < data.nbContours; i++) {
        uint end = b_FontContourEnds[endOfContourPtr];
        ivec2 p0_ = b_GlyphPoints[start];
        ivec2 p0 = p0_;

        // Get lastN of very last spline
        // FLAW : if sur-last spline is flat horizontal, then this does not give the requested information (but fuck them bad font makers bruh, why would u even put 2 straight lines next to each other???)
        int lastN2 = 0;
        Spline sp0 = Spline(b_GlyphPoints[end - 1], b_GlyphPoints[end], p0);

        if (pos.x < sp0.p0.x || pos.x < sp0.p1.x || pos.x < sp0.p2.x) {
            ivec3 Mmc = splineCategory(sp0);
            if (Mmc.z == -1) {
                sp0 = Spline(b_GlyphPoints[end - 3], b_GlyphPoints[end - 2], sp0.p0);
                if (pos.x < sp0.p0.x || pos.x < sp0.p1.x || pos.x < sp0.p2.x) 
                    Mmc = splineCategory(sp0);
            }

            if (Mmc.z != -1 && pos.y == sp0.p2.y) lastN2 = Mmc.z == 0 || Mmc.z == 3 ? 1 : -1;
        }

        #define CONTINUE() { lastN2 = 0; continue; }
        for (uint j = start; j <= end; j += 2) {
            ivec2 p1 = b_GlyphPoints[j + 1];
            ivec2 p2 = (j == end - 1) ? p0_ : b_GlyphPoints[j + 2];
            Spline sp = Spline(p0, p1, p2);
            p0 = p2;

            // Eliminate splines behind ray
            if (pos.x > sp.p0.x && pos.x > sp.p1.x && pos.x > sp.p2.x) CONTINUE()

            // Get spline category (going up, down or both)
            ivec3 Mmc = splineCategory(sp);
            int maxY = Mmc.x;
            int minY = Mmc.y;
            int category = Mmc.z;

            // If flat curve, truly ignore
            if (category == -1) continue;

            // If curve outside ray, ignore
            if (pos.y < minY || pos.y > maxY) CONTINUE()

            int n = intersectSpline(pos, sp, category, lastN2);
            nbIntersections += n;
            lastN2 = Mmc.z == 0 || Mmc.z == 3 ? 1 : -1;
        }

        endOfContourPtr++;
        start = end + 1;
    }

    return nbIntersections;
}

bool insideText(vec2 uv, uint glyphIdx) {
    if (glyphIdx == 0u) return false;
    GlyphData data = b_GlyphData[glyphIdx];
    vec2 UV = uv * data.coords.zw + data.coords.xy;

    int hitNb = hitGlyph(UV, data);
    return hitNb != 0;
}