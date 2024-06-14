
#ifndef INKLING_MATH_H
#define INKLING_MATH_H

#include <inkling/types.h>
#include <inkling/marks.h>
#include <inkling/reflect.h>
#include <math.h>

typedef union {
    struct {
        f32 x;
        f32 y;
    };
    f32 m[2];
} vec2;

extern ink_readonly ink_typeInfo vec2TypeInfo;

#define VEC2_X (vec2){1.0, 0.0}
#define VEC2_Y (vec2){0.0, 1.0}

typedef union {
    struct {
        f32 x;
        f32 y;
        f32 z;
        f32 w;
    };
    struct {
        f32 r;
        f32 g;
        f32 b;
        f32 a;
    };
    f32 m[4];
} vec4;

extern ink_readonly ink_typeInfo vec4TypeInfo;

#define VEC4_X (vec4){1.0, 0.0, 0.0, 0.0}
#define VEC4_Y (vec4){0.0, 1.0, 0.0, 0.0}
#define VEC4_Z (vec4){0.0, 0.0, 1.0, 0.0}
#define VEC4_W (vec4){0.0, 0.0, 0.0, 1.0}

typedef union {
    struct { // basis vector form
        vec4 x;
        vec4 y;
        vec4 z;
        vec4 w;
    };
    f32 m[16]; // column-major order
} mat4;

extern ink_readonly ink_typeInfo mat4TypeInfo;

#define MAT4_IDENTITY (mat4){.x = VEC4_X, .y = VEC4_Y, .z = VEC4_Z, .w = VEC4_W}

vec4 ink_transform(mat4* mat, vec4 vec);
mat4 ink_mulMat4(mat4* first, mat4* second);
mat4 ink_calc2DTransform(vec2 position, vec2 scale, f32 angle);
mat4 ink_calcOrthoCamMatrix(f32 aspect, f32 viewportHeight, f32 zNear, f32 zFar, vec2 camPos);

#endif
