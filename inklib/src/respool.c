
#include <inkling/respool.h>
#include <inkling/memory.h>

typedef struct {
    u32 magic;
    u32 nextFree;
} entryHeader;

usize calcEntrySize(ink_respool* pool) {
    return sizeof(entryHeader) + pool->itemType->size;
}

ink_ref ink_allocRes(ink_respool* pool) {

    u32 idx;
    entryHeader* entry;
    usize entrySize = calcEntrySize(pool);
    if(pool->firstFree == U32_MAX) {
        pool->len++;
        if(pool->len > pool->cap) {
            usize newCap = pool->cap == 0 ? 8 : pool->cap * 2;
            usize oldSize = pool->cap * entrySize;
            usize newSize = newCap * entrySize;
            pool->data = ink_realloc(pool->data, oldSize, newSize);
        }
        idx = pool->len - 1;
        entry = pool->data + idx * entrySize;
    } else {
        idx = pool->firstFree;
        entry = pool->data + idx * entrySize;
        pool->firstFree = entry->nextFree;
    }
    entry->magic = pool->currMagic;
    pool->currMagic++;
    return (ink_ref){
        .idx = idx,
        .magic = pool->currMagic - 1
    };
}

void* ink_getRes(ink_respool* pool, ink_ref ref) {
    if(ref.idx >= pool->len)
        return NULL;
    usize entrySize = calcEntrySize(pool);
    entryHeader* entry = pool->data + entrySize * ref.idx;
    if(entry->magic != ref.magic)
        return NULL;
    void* entryData = (void*)entry + sizeof(entryHeader);
    return entryData;
}

void ink_dropRes(ink_respool* pool, ink_ref ref) {
    if(ref.idx >= pool->len)
        return;
    usize entrySize = calcEntrySize(pool);
    entryHeader* entry = pool->data + entrySize * ref.idx;
    if(entry->magic != ref.magic)
        return;
    entry->magic = 0;
    entry->nextFree = pool->firstFree;
    pool->firstFree = ref.idx;
    return;
}
