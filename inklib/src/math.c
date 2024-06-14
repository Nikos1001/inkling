
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
