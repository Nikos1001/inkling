
#ifndef INKDEV_BUILD_H
#define INKDEV_BUILD_H

#include <inkling/common.h>

ink_str rebuild(ink_arena* arena, u32 buildId, bool* success);
void build(ink_arena* arena);

#endif
