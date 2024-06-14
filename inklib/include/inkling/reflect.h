
#ifndef INKLING_REFLECT_H
#define INKLING_REFLECT_H

#include <inkling/types.h>
#include <inkling/marks.h>

#include <stddef.h> // offsetof

struct ink_typeInfo;

typedef struct {
    const char* name;
    usize offset;
    struct ink_typeInfo* type;
} ink_field;

typedef enum {
    INK_GFX_UNIFORM_TYPE_NONE,
    INK_GFX_UNIFORM_TYPE_MAT4,
    INK_GFX_UNIFORM_TYPE_SAMPLER
} ink_gfxUniformType;

typedef struct ink_typeInfo {
    usize size;

    ink_gfxUniformType gfxUniformType;
    
    usize nFields;
    ink_field fields[];
} ink_typeInfo;

extern ink_readonly ink_typeInfo u8TypeInfo;
extern ink_readonly ink_typeInfo i32TypeInfo;
extern ink_readonly ink_typeInfo u32TypeInfo;
extern ink_readonly ink_typeInfo f32TypeInfo;
extern ink_readonly ink_typeInfo f64TypeInfo;
extern ink_readonly ink_typeInfo usizeTypeInfo;

void ink_transferData(void* from, ink_typeInfo* fromType, void* to, ink_typeInfo* toType);

#endif
