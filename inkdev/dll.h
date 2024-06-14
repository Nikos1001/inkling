
#ifndef INKDEV_DLL_H
#define INKDEV_DLL_H

#include <inkling/types.h>
#include <inkling/reflect.h>

typedef struct {
    void* dll;

    void (*update)(f32, void*);
    void (*reload)(void*);
    ink_typeInfo* stateTypeInfo;
} gameDLL;

gameDLL loadDLL(const char* path); 
void dropDLL(gameDLL* dll);

#endif
