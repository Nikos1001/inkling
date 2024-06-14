
#include <inkling/types.h>
#include <inkling/arena.h>
#include <inkling/memory.h>
#include <inkling/panic.h>

ink_arena ink_makeArena(usize size) {
    void* alloc = ink_malloc(size);
    return (ink_arena){
        .base = alloc,
        .curr = alloc,
        .size = size
    };
}

void ink_dropArena(ink_arena* arena) {
    ink_free(arena->base, arena->size);
}

void* ink_arenaAlloc(ink_arena* arena, usize size) {
    if(arena->curr - arena->base + size > arena->size) {
        // TODO: potentially growing arena?
        ink_panic("Arena ran out of memory.");
    }

    void* alloc = arena->curr;
    arena->curr += size;
    memset(alloc, 0, size);
    return alloc;
}

void ink_arenaClear(ink_arena* arena) {
    arena->curr = arena->base;
}