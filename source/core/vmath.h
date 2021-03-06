/* date = September 29th 2021 10:03 am */

#ifndef VMATH_H
#define VMATH_H

#define EPSILON 0.001f
#define PI 3.141592653589f
#define HALF_PI 1.570796326794f
#define DEG_TO_RAD 0.0174532925f
#define RAD_TO_DEG 57.2957795131

typedef struct vec2 { f32 x; f32 y;               } vec2;
typedef struct vec3 { f32 x; f32 y; f32 z;        } vec3;
typedef struct vec4 { f32 x; f32 y; f32 z; f32 w; } vec4;

typedef struct mat3 { f32 a[3*3]; } mat3;
typedef struct mat4 { f32 a[4*4]; } mat4;

typedef struct rect {
    f32 x; f32 y; f32 w; f32 h;
} rect;

//~ Math Utilities

void animate_f32exp(f32* val, f32 target, f32 speed, f32 dt);

//~ Inline Initializers

static inline vec2 vec2_init(f32 x, f32 y) { return (vec2) { .x = x, .y = y }; }
static inline vec3 vec3_init(f32 x, f32 y, f32 z) { return (vec3) { .x = x, .y = y, .z = z }; }
static inline vec4 vec4_init(f32 x, f32 y, f32 z, f32 w) { return (vec4) { .x = x, .y = y, .z = z, .w = w }; }
static inline rect rect_init(f32 x, f32 y, f32 w, f32 h) { return (rect) { .x = x, .y = y, .w = w, .h = h }; }

static inline u16 mat3_idx(u16 x, u16 y) { return y * 3 + x; }
static inline u16 mat4_idx(u16 x, u16 y) { return y * 4 + x; }

static inline f64 radians(f32 deg) { return (f64) (deg * DEG_TO_RAD); }
static inline f32 degrees(f64 rad) { return (f32) (rad * RAD_TO_DEG); }

//~ Vector Functions

vec2 vec2_add(vec2 a, vec2 b);
vec2 vec2_sub(vec2 a, vec2 b);
vec2 vec2_scale(vec2 a, f32 s);
vec2 vec2_clamp(vec2 vec, rect quad);

vec3 vec3_add(vec3 a, vec3 b);
vec3 vec3_sub(vec3 a, vec3 b);
vec3 vec3_scale(vec3 a, f32 s);

vec4 vec4_add(vec4 a, vec4 b);
vec4 vec4_sub(vec4 a, vec4 b);
vec4 vec4_scale(vec4 a, f32 s);

vec3 vec3_mul(vec3 a, mat3 m);
vec4 vec4_mul(vec4 a, mat4 m);

//~ Matrix Functions

mat3 mat3_identity();
mat4 mat4_identity();

mat3 mat3_mul(mat3 a, mat3 b);
void mat3_set(mat3* mat, mat3 o);

mat3 mat3_translate(vec2 v);
mat3 mat3_rotate(f32 r);
mat3 mat3_scalev(vec2 s);
mat3 mat3_scalef(f32 s);

mat4 mat4_mul(mat4 a, mat4 b);
void mat4_set(mat4* mat, mat4 o);
mat4 mat4_transpose(mat4 a);

mat4 mat4_translate(vec3 v);
mat4 mat4_ortho(f32 left, f32 right, f32 top, f32 bottom, f32 near, f32 far);

//~ Rect Functions

b8   rect_overlaps(rect a, rect b);
b8   rect_contained_by_rect(rect a, rect b);
rect rect_get_overlap(rect a, rect b);
rect rect_uv_cull(rect pos, rect uv, rect quad);

#endif //VMATH_H
