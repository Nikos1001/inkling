
#ifndef INKDEV_DLL_H
#define INKDEV_DLL_H

#include <inkling/types.h>
#include <inkling/reflect.h>

typedef struct {
    void* dll;

    void (*init)(void* gameState);
    void (*update)(f32 dt, void* gameState);
    void (*reload)(void* gameState);
    ink_typeInfo* stateTypeInfo;
} gameDLL;

gameDLL loadDLL(const char* path, bool* success); 
void dropDLL(gameDLL* dll);

#endif
