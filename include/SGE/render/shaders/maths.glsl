/// @brief The ratio of the circumference to the diameter
#define PI (3.1415926535897931)
/// @brief The ratio of the circumference to the radius
#define TAU (6.2831853071796)
/// @brief The ratio of 2*PI rad over 360°
#define DEG_TO_RAD (PI / 180.0)
/// @brief The ratio of 360° to 2*PI rad
#define RAD_TO_DEG (180.0 / PI)

/// @brief Euler's number, e
#define EULER_NUM (2.7182818284590452)

#define FLOAT_MIN (1e-37f)
#define FLOAT_MAX (1e38f)
#define FLOAT_INF (uintBitsToFloat(0x7F800000))
#define FLOAT_NINF (uintBitsToFloat(0xFF800000))

uint PCG_hash(uint i) {
    uint state = i * 747796405u + 2891336453u;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}
float rand(inout uint seed) {
    seed = PCG_hash(seed);
    uint usable = seed & ((1u << 23u) - 1u) | (((1u << 7u) - 1u) << 23u);
    return uintBitsToFloat(usable) - 1.0;
}



///// POLYNOMS

#define UNDEFINED_ROOT FLOAT_MAX
float solveLinear(float a, float b, out uint solutionCount) {
    if (a == 0.0) {
        solutionCount = 0u;
        return UNDEFINED_ROOT;
    }
    solutionCount = 1u;
    return -b / a;
}
vec2 solveQuadratic(float a, float b, float c, out uint solutionCount) {

    if (a == 0.0) return vec2(solveLinear(b, c, solutionCount), UNDEFINED_ROOT);
    // a is != 0 from this point on

    float hb = b * 0.5; // Half of B
    float qdet = hb * hb - a * c; // Quater of Determinant
    if (qdet < 0.0) { solutionCount = 0u; return vec2(UNDEFINED_ROOT, UNDEFINED_ROOT); }
    if (qdet == 0.0) { solutionCount = 1u; return vec2(-hb / a, UNDEFINED_ROOT); }

    float hdelta = sqrt(qdet); // Half of sqrt(Determinant)

    solutionCount = 2u;
    return vec2(-hdelta - hb, hdelta - hb) / a;
}
vec3 solveCubic_depressed(float p, float q, out uint solutionCount) {

    const float inv3 = 0.33333333333333331; // 1.0 / 3.0
    const float sqrt3 = 1.7320508075688772; // Sqrt(3.0)

    float disc = - (4.0*p*p*p + 27.0*q*q);
    if (disc < 0.0) {
        float hq = q * 0.5, // Half q
          pOver3 = p * inv3; 

        solutionCount = 1u;
        return vec3(pow(-hq + sqrt(hq*hq + pOver3*pOver3*pOver3), inv3), UNDEFINED_ROOT, UNDEFINED_ROOT);
    }
    if (disc == 0.0) {
        if (p == 0.0) return vec3(0);
        float invP = 1.0 / p;
        float threeQ = 3.0*q;
        float r2 = -threeQ * (0.5*invP);

        solutionCount = 2u;
        return vec3(threeQ * invP, r2, r2);
    }
    /* if (disc > 0.0) */ {
        float u0 = sqrt(-p*inv3);
        float u = acos((1.5*q) / (p*u0));

        vec3 r = (2.0*u0) * vec3(
            cos(inv3*(u - TAU*0.0)),
            cos(inv3*(u - TAU*1.0)),
            cos(inv3*(u - TAU*2.0))
        );

        solutionCount = 2u + (r.y != r.z ? 1u : 0u);
        return r;
    }
}
vec3 solveCubic(float a, float b, float c, float d, out uint solutionCount) {

    if (a == 0.0) return vec3(solveQuadratic(b, c, d, solutionCount), UNDEFINED_ROOT);

    float inv3 = 0.33333333333333331; // 1.0 / 3.0

    // Transform to depressed cubic
    float invA = 1.0 / a;
    float inv3A = inv3 * invA;
    float b2 = b*b, ac = a*c, a2 = a*a, inv3A2 = inv3A*invA;

    float p = (3.0*ac - b2) * (inv3A2);
    float q = (2.0*b2 - 9.0*ac*b + 27.0*a2*d) * (inv3A2*inv3A);
    float rootOffset = b*inv3A;

    return solveCubic_depressed(p, q, solutionCount) - vec3(rootOffset);
}
vec4 solveQuartic(float A, float B, float C, float D, float E, out uint solutionCount) {

    if (A == 0.0) return vec4(solveCubic(B, C, D, E, solutionCount), UNDEFINED_ROOT);

    // Transform to depressed quartic
    float invA = 1.0 / A, B2 = B*B;
    float a = C - 0.375*B2;
    float b = D - 0.5*B*(C + B2*0.25);
    float c = E - 0.25*B*(D + B*C*0.25 - (3.0/64.0)*B2*B);

    ///// Using Ferrari's method
    // Forming the cubic
    uint solutionCountCubic;
    float y = solveCubic(1.0, 2.5*a, 2.0*a - c, 0.5*a*(a*a - c) - b*b*0.125, solutionCountCubic).x;
    if (solutionCountCubic == 0u) { solutionCount = 0u; return vec4(UNDEFINED_ROOT); }

    // Generating solutions
    float twoY = 2.0*y;
    float W = sqrt(a + twoY);

    float d0_a = -3.0*a - twoY;
    float d0_b = 2.0*b/W;

    float d1_a = d0_a - d0_b;
    float d1_b = d0_a + d0_b;

    float d2_a = d1_a < 0.0 ? -1.0 : sqrt(d1_a); 
    float d2_b = d1_b < 0.0 ? -1.0 : sqrt(d1_b); 

    vec4 r = vec4(0);
    if (d2_a < 0.0) {
        if (d2_b < 0.0) { solutionCount = 0u; return vec4(UNDEFINED_ROOT); }
        solutionCount = 2u;
        return vec4(W - d2_b, W + d2_b, UNDEFINED_ROOT, UNDEFINED_ROOT) * 0.5 - B*invA*0.25;
    }
    else if (d2_b < 0.0) {
        solutionCount = 2u;
        return vec4(-W - d2_a, -W + d2_a, UNDEFINED_ROOT, UNDEFINED_ROOT) * 0.5 - B*invA*0.25;
    }
    
    solutionCount = 4u;
    return vec4(-W - d2_a, -W + d2_a, +W - d2_b, +W + d2_b) * 0.5 - B*invA*0.25;
}



///// VECTOR

#define dot2(v) dot(v, v)

vec2 randUnit2(inout uint seed) {
    float theta = rand(seed) * TAU;
    return vec2(cos(theta), sin(theta));
}
vec3 randUnit3(inout uint seed) {
    float theta = rand(seed) * TAU;
    float z = 2.0 * rand(seed) - 1.0;
    float l = sqrt(clamp(1.0 - z*z, 0, 1));
    return vec3(sin(theta)*l, cos(theta)*l, z);
}

float hash21(vec2 p) {
    return fract(43757.5453*sin(dot(p, vec2(12.9898,78.233))));
}
float hash31(vec3 p) {
    return fract(43757.5453*sin(dot(p, vec3(12.9898,78.233, 9.8192))));
}

vec2 hash22(vec2 v) {
    return vec2(hash21(v), hash31(cross(v.xyx, vec3(5.82, 13.0182, 8.61))));
}
vec3 hash33(vec3 v) {
    return vec3(hash31(v), hash31(cross(v.yzx, vec3(5.82, 13.0182, 8.61))), hash31(cross(v.zxy, vec3(5.82, 13.0182, 8.61).yxz)));
}

vec2 hash2Unit2(vec2 v) {
    float theta = hash21(v)*TAU;
    return vec2(cos(theta), sin(theta));
}
vec3 hash3Unit3(vec3 v) {
    float a = hash31(v) * TAU;
    float b = (hash31(cross(v.yzx, vec3(1892.82, 73.0182, 162.5229))) - 0.5) * PI;

    float cosA = cos(a);
    return vec3(cosA * cos(b), cosA * sin(b), sin(a));
}



///// NOISE

float noise2D_voronoi(vec2 p) {
    vec2 ip = floor(p);
    vec2 fp = fract(p);
    
    float minDist = 9.0;
    for(int x = -1; x <= 1; x++)
    for(int y = -1; y <= 1; y++) {
        vec2 offset = vec2(x, y);
        vec2 randomPoint = hash22(ip + offset) + offset;
        vec2 mid = (randomPoint + fp) * 0.5;
        
        vec2 diff = mid - fp;
        float md = dot2(diff);
        
        minDist = min(minDist, md);
    }
    
    return sqrt(minDist);
}
float noise3D_voronoi(vec3 p) {
    vec3 ip = floor(p);
    vec3 fp = fract(p);
    
    float minDist = 27.0;
    for(int x = -1; x <= 1; x++)
    for(int y = -1; y <= 1; y++)
    for(int z = -1; z <= 1; z++) {
        vec3 offset = vec3(x, y, z);
        vec3 randomPoint = hash33(ip + offset) + offset;
        vec3 mid = (randomPoint + fp) * 0.5;
        
        vec3 diff = mid - fp;
        float md = dot2(diff);
        
        minDist = min(minDist, md);
    }
    
    return sqrt(minDist);
}

float noise2D_perlin(vec2 p) {

    vec2 ip = floor(p);
    vec2 fp = (p - ip);
    vec2 sfp = fp*fp*(3.0-2.0*fp);
    vec2 e = vec2(1, 0);

    // corner random vectors
    vec2 cr1 = hash2Unit2(ip + e.yy);
    vec2 cr2 = hash2Unit2(ip + e.xy);
    vec2 cr3 = hash2Unit2(ip + e.yx);
    vec2 cr4 = hash2Unit2(ip + e.xx);
    
    // dot products lerped in x
    float d1 = mix(dot(cr1, fp - e.yy), dot(cr2, fp - e.xy), sfp.x);
    float d2 = mix(dot(cr3, fp - e.yx), dot(cr4, fp - e.xx), sfp.x);

    // dot products lerped in y
    return mix(d1, d2, sfp.y);
}
float noise3D_perlin(vec3 p) {

    vec3 ip = floor(p);
    vec3 fp = (p - ip);
    vec3 sfp = fp*fp*(3.0-2.0*fp);
    vec2 e = vec2(1, 0);

    // corner random vectors
    vec3 cr1 = hash3Unit3(ip + e.yyy);
    vec3 cr2 = hash3Unit3(ip + e.xyy);
    vec3 cr3 = hash3Unit3(ip + e.yxy);
    vec3 cr4 = hash3Unit3(ip + e.xxy);

    vec3 cr5 = hash3Unit3(ip + e.yyx);
    vec3 cr6 = hash3Unit3(ip + e.xyx);
    vec3 cr7 = hash3Unit3(ip + e.yxx);
    vec3 cr8 = hash3Unit3(ip + e.xxx);
    
    // dot products lerped in x
    float d1 = mix(dot(cr1, fp - e.yyy), dot(cr2, fp - e.xyy), sfp.x);
    float d2 = mix(dot(cr3, fp - e.yxy), dot(cr4, fp - e.xxy), sfp.x);

    float d3 = mix(dot(cr5, fp - e.yyx), dot(cr6, fp - e.xyx), sfp.x);
    float d4 = mix(dot(cr7, fp - e.yxx), dot(cr8, fp - e.xxx), sfp.x);

    // dot products lerped in y
    float d5 = mix(d1, d3, sfp.y);
    float d6 = mix(d2, d4, sfp.y);

    // dot products lerped in z
    return mix(d1, d2, sfp.z);
}