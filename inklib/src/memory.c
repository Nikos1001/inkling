
#include <inkling/types.h>

void* ink_realloc(void* ptr, usize oldSize, usize newSize) {
    if(newSize == 0) {
        free(ptr);
        return NULL;
    }
    if(ptr == NULL)
        return malloc(newSize);
    return realloc(ptr, newSize);
}

void* ink_malloc(usize size) {
    return ink_realloc(NULL, 0, size);
}

void ink_free(void* ptr, usize size) {
    ink_realloc(ptr, size, 0);
}
