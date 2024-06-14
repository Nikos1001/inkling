
#ifndef INKLING_ARENA_H
#define INKLING_ARENA_H

#include <inkling/types.h>

typedef struct {
    void* base;
    void* curr;
    usize size;
} ink_arena;

ink_arena ink_makeArena(usize size);
void ink_dropArena(ink_arena* arena);
void* ink_arenaAlloc(ink_arena* arena, usize size);
void ink_arenaClear(ink_arena* arena);

#define INK_ARENA_ALLOC(arena, type) (type*)ink_arenaAlloc((arena), sizeof(type))

#endif
