#include <math.h>

void animate_f32exp(f32* val, f32 target, f32 speed, f32 dt) {
    f32 delta = target - (*val);
    *val += delta * dt * speed;
}

vec2 vec2_add(vec2 a, vec2 b) {
    return (vec2) { .x = a.x + b.x, .y = a.y + b.y };
}

vec2 vec2_sub(vec2 a, vec2 b) {
    return (vec2) { .x = a.x - b.x, .y = a.y - b.y };
}

vec2 vec2_scale(vec2 a, f32 s) {
    return (vec2) { .x = a.x * s, .y = a.y * s };
}

vec3 vec3_add(vec3 a, vec3 b) {
    return (vec3) { .x = a.x + b.x, .y = a.y + b.y, .z = a.z + b.z };
}

vec3 vec3_sub(vec3 a, vec3 b) {
    return (vec3) { .x = a.x - b.x, .y = a.y - b.y, .z = a.z - b.z };
}

vec3 vec3_scale(vec3 a, f32 s) {
    return (vec3) { .x = a.x * s, .y = a.y * s, .z = a.z * s };
}

vec4 vec4_add(vec4 a, vec4 b) {
    return (vec4) { .x = a.x + b.x, .y = a.y + b.y, .z = a.z + b.z, .w = a.w + b.w };
}

vec4 vec4_sub(vec4 a, vec4 b) {
    return (vec4) { .x = a.x - b.x, .y = a.y - b.y, .z = a.z - b.z, .w = a.w - b.w };
}

vec4 vec4_scale(vec4 a, f32 s) {
    return (vec4) { .x = a.x * s, .y = a.y * s, .z = a.z * s, .w = a.w * s };
}

vec3 vec3_mul(vec3 a, mat3 m) {
    return (vec3) {
        .x = (a.x * m.a[mat3_idx(0, 0)] + a.y * m.a[mat3_idx(1, 0)] + a.z * m.a[mat3_idx(2, 0)]),
        .y = (a.x * m.a[mat3_idx(0, 1)] + a.y * m.a[mat3_idx(1, 1)] + a.z * m.a[mat3_idx(2, 1)]),
        .z = (a.x * m.a[mat3_idx(0, 2)] + a.y * m.a[mat3_idx(1, 2)] + a.z * m.a[mat3_idx(2, 2)])
    };
}

vec4 vec4_mul(vec4 a, mat4 m) {
    return (vec4) {
        .x = (a.x * m.a[mat4_idx(0, 0)] + a.y * m.a[mat4_idx(1, 0)] + a.z * m.a[mat4_idx(2, 0)] + a.w * m.a[mat4_idx(3, 0)]),
        .y = (a.x * m.a[mat4_idx(0, 1)] + a.y * m.a[mat4_idx(1, 1)] + a.z * m.a[mat4_idx(2, 1)] + a.w * m.a[mat4_idx(3, 1)]),
        .z = (a.x * m.a[mat4_idx(0, 2)] + a.y * m.a[mat4_idx(1, 2)] + a.z * m.a[mat4_idx(2, 2)] + a.w * m.a[mat4_idx(3, 2)]),
        .w = (a.x * m.a[mat4_idx(0, 3)] + a.y * m.a[mat4_idx(1, 3)] + a.z * m.a[mat4_idx(2, 3)] + a.w * m.a[mat4_idx(3, 3)])
    };
}

mat3 mat3_identity() {
    return (mat3) {
        .a = {
            1.f, 0.f, 0.f,
            0.f, 1.f, 0.f,
            0.f, 0.f, 1.f,
        }
    };
}

mat4 mat4_identity() {
    return (mat4) {
        .a = {
            1.f, 0.f, 0.f, 0.f,
            0.f, 1.f, 0.f, 0.f,
            0.f, 0.f, 1.f, 0.f,
            0.f, 0.f, 0.f, 1.f,
        }
    };
}

mat3 mat3_mul(mat3 a, mat3 b) {
    mat3 result = mat3_identity();
    for (u16 j = 0; j < 3; j++) {
        for (u16 i = 0; i < 3; i++) {
            result.a[mat3_idx(i, j)] =
                a.a[mat3_idx(i, 0)] * b.a[mat3_idx(0, j)] +
                a.a[mat3_idx(i, 1)] * b.a[mat3_idx(1, j)] +
                a.a[mat3_idx(i, 2)] * b.a[mat3_idx(2, j)];
        }
    }
    return result;
}

void mat3_set(mat3* mat, mat3 o) {
    for (u16 j = 0; j < 3; j++) {
        for (u16 i = 0; i < 3; i++) {
            mat->a[mat3_idx(i, j)] = o.a[mat3_idx(i, j)];
        }
    }
}

mat3 mat3_translate(vec2 v) {
    return (mat3) {
        .a = {
            1.f, 0.f, v.x,
            0.f, 1.f, v.y,
            0.f, 0.f, 1.f,
        }
    };
}

mat3 mat3_rotate(f32 r) {
    return (mat3) {
        .a = {
            cos(radians(r)), -sin(radians(r)), 0.f,
            sin(radians(r)),  cos(radians(r)), 0.f,
            0.f,              0.f,             1.f,
        }
    };
}

mat3 mat3_scalev(vec2 s) {
    return (mat3) {
        .a = {
            s.x, 0.f, 0.f,
            0.f, s.y, 0.f,
            0.f, 0.f, 1.f,
        }
    };
}

mat3 mat3_scalef(f32 s) {
    return (mat3) {
        .a = {
            s,   0.f, 0.f,
            0.f, s,   0.f,
            0.f, 0.f, 1.f,
        }
    };
}

mat4 mat4_mul(mat4 a, mat4 b) {
    mat4 result = mat4_identity();
    for (u16 j = 0; j < 4; j++) {
        for (u16 i = 0; i < 4; i++) {
            result.a[mat4_idx(i, j)] =
                a.a[mat4_idx(i, 0)] * b.a[mat4_idx(0, j)] +
                a.a[mat4_idx(i, 1)] * b.a[mat4_idx(1, j)] +
                a.a[mat4_idx(i, 2)] * b.a[mat4_idx(2, j)] +
                a.a[mat4_idx(i, 3)] * b.a[mat4_idx(3, j)];
        }
    }
    return result;
}

void mat4_set(mat4* mat, mat4 o) {
    for (u16 j = 0; j < 4; j++) {
        for (u16 i = 0; i < 4; i++) {
            mat->a[mat4_idx(i, j)] = o.a[mat4_idx(i, j)];
        }
    }
}

mat4 mat4_translate(vec3 v) {
    return (mat4) {
        .a = {
            1.f, 0.f, 0.f, v.x,
            0.f, 1.f, 0.f, v.y,
            0.f, 0.f, 1.f, v.z,
            0.f, 0.f, 0.f, 1.f,
        }
    };
}

mat4 mat4_transpose(mat4 a) {
    mat4 ret = mat4_identity();
    for (u16 j = 0; j < 4; j++) {
        for (u16 i = 0; i < 4; i++) {
            ret.a[mat4_idx(i, j)] = a.a[mat4_idx(j, i)];
        }
    }
    return ret;
}

mat4 mat4_ortho(f32 left, f32 right, f32 top, f32 bottom, f32 near, f32 far) {
    f32 width = right - left;
    f32 height = top - bottom;
    f32 depth = far - near;
    return (mat4) {
        .a = {
            2.f / width, 0.f,          0.f,          -(right + left) / width,
            0.f,         2.f / height, 0.f,          -(top + bottom) / height,
            0.f,         0.f,          -2.f / depth, -(far + near) / depth,
            0.f,         0.f,          0.f,          1.f,
        }
    };
}
