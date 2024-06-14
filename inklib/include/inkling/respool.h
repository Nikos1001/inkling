
#ifndef INKLING_RESPOOL_H
#define INKLING_RESPOOL_H

#include <inkling/types.h>
#include <inkling/reflect.h>

typedef struct {
    ink_typeInfo* itemType;
    usize len;
    usize cap;
    u32 currMagic;
    u32 firstFree;
    void* data;
} ink_respool;

typedef struct {
    u32 idx;
    u32 magic;
} ink_ref;


#define ink_makeRespool(type) (ink_respool){ .itemType = (type), .len = 0, .cap = 0, .currMagic = 1, .firstFree = U32_MAX, .data = NULL }
ink_ref ink_allocRes(ink_respool* pool);
void* ink_getRes(ink_respool* pool, ink_ref ref);
void ink_dropRes(ink_respool* pool, ink_ref ref);

#endif
