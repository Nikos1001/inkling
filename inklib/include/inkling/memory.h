
#ifndef INKLING_MEMORY_H
#define INKLING_MEMORY_H

#include <inkling/types.h>

void* ink_realloc(void* ptr, usize oldSize, usize newSize);
void* ink_malloc(usize size);
void ink_free(void* ptr, usize size);

#endif
