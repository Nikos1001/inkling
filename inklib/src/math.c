
#include <inkling/math.h>
#include <inkling/reflect.h>

ink_readonly ink_typeInfo vec2TypeInfo = {
    .size = sizeof(vec2),
};

ink_readonly ink_typeInfo vec4TypeInfo = {
    .size = sizeof(vec4),
};

ink_readonly ink_typeInfo mat4TypeInfo = {
    .size = sizeof(vec4),
    .gfxUniformType = INK_GFX_UNIFORM_TYPE_MAT4
};

vec4 ink_transform(mat4* mat, vec4 vec) {
    return (vec4){
        vec.x * mat->x.x + vec.y * mat->y.x + vec.z * mat->z.x + vec.w * mat->w.x,
        vec.x * mat->x.y + vec.y * mat->y.y + vec.z * mat->z.y + vec.w * mat->w.y,
        vec.x * mat->x.z + vec.y * mat->y.z + vec.z * mat->z.z + vec.w * mat->w.z,
        vec.x * mat->x.w + vec.y * mat->y.w + vec.z * mat->z.w + vec.w * mat->w.w
    };
}

mat4 ink_mulMat4(mat4* first, mat4* second) {
    return (mat4){
        .x = ink_transform(second, ink_transform(first, VEC4_X)),
        .y = ink_transform(second, ink_transform(first, VEC4_Y)),
        .z = ink_transform(second, ink_transform(first, VEC4_Z)),
        .w = ink_transform(second, ink_transform(first, VEC4_W)),
    };
}

mat4 ink_calc2DTransform(vec2 position, vec2 scale, f32 angle) {
    return (mat4){
        .x = (vec4){scale.x * cosf(angle), scale.x * sinf(angle), 0.0, 0.0},
        .y = (vec4){-scale.y * sinf(angle), scale.y * cosf(angle), 0.0, 0.0},
        .z = VEC4_Z,
        .w = (vec4){position.x, position.y, 0.0, 1.0}
    };
}

mat4 ink_calcOrthoCamMatrix(f32 aspect, f32 viewportHeight, f32 zNear, f32 zFar, vec2 camPos) {
    viewportHeight /= 2.0;

    f32 viewportWidth = aspect * viewportHeight;
    f32 zRange = zFar - zNear;

    mat4 projection = {
        .x = (vec4){1.0 / viewportWidth, 0.0, 0.0, 0.0},
        .y = (vec4){0.0, 1.0 / viewportHeight, 0.0, 0.0},
        .z = (vec4){0.0, 0.0, 1.0 / zRange, 0.0},
        .w = (vec4){-camPos.x / viewportWidth, -camPos.y / viewportHeight, -1.0 - zNear / zRange, 1.0}
    };
    
    return projection;
}
