
#include "dll.h"
#include "../inklib/dev/api.h"

#include <inkling/log.h>
#include <dlfcn.h>
#include <stdio.h>

const char* logPrefix[] = {
    [INK_LOG_INFO] = "\033[0m[info] ",
    [INK_LOG_WARNING] = "\033[0;33m[warning] ",
    [INK_LOG_ERROR] = "\033[0;31m[error] "
};

void inkdevLog(void* logger, const char* msg, const char* file, u32 line, ink_log_type type) {
    printf("%s%s\n", logPrefix[type], msg);
    printf("From %s:%d\n", file, line);
}

ink_logger logger = {
    .log = inkdevLog
};

gameDLL loadDLL(const char* path) {

    gameDLL dll = {
        .dll = NULL,
        .stateTypeInfo = NULL,
        .update = NULL
    };

    dll.dll = dlopen(path, RTLD_GLOBAL | RTLD_LAZY); 
    if(dll.dll == NULL) {
        ink_error("Could not load dll.");
        return dll;
    }

    void (*initApi)(inkapi api) = dlsym(dll.dll, "inkapi_init");
    if(initApi == NULL) {
        ink_error("inkapi_init missing from dll.");
        return dll;
    }
    initApi((inkapi) {
        .logger = &logger,
        .setWindowTitle = ink_setWindowTitle,
        .getWindowAspect = ink_getWindowAspect,
        .keyDown = ink_keyDown,
        .makeBuffer = ink_makeBuffer,
        .makeBindings = ink_makeBindings,
        .dropBindings = ink_dropBindings,
        .uploadBufferData = ink_uploadBufferData,
        .dropBuffer = ink_dropBuffer,
        .makeTexture = ink_makeTexture,
        .uploadTextureData = ink_uploadTextureData,
        .dropTexture = ink_dropTexture,
        .makeShader = ink_makeShader,
        .dropShader = ink_dropShader,
        .beginPass = ink_beginPass,
        .applyBindings = ink_applyBindings,
        .applyPipeline = ink_applyPipeline,
        .updatePipelineUniforms = ink_updatePipelineUniforms,
        .draw = ink_draw
    });

    ink_typeInfo* (*stateTypeInfo)() = dlsym(dll.dll, "stateTypeInfo");
    if(stateTypeInfo != NULL) {
        dll.stateTypeInfo = stateTypeInfo();
    }

    dll.update = dlsym(dll.dll, "update");
    dll.reload = dlsym(dll.dll, "reload");

    return dll;
}

void dropDLL(gameDLL* dll) {
    dlclose(dll->dll);
    dll->dll = NULL;
    dll->stateTypeInfo = NULL;
    dll->update = NULL;
}
