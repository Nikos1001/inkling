
#include <stdio.h>
#include <inkling/common.h>
#include <inkling/log.h>
#include <inkling/file.h>
#include <inkling/input.h>

#include "dll.h"
#include "build.h"

const char* buildLogPrefix[] = {
    [INK_LOG_INFO] = "\033[0m",
    [INK_LOG_WARNING] = "\033[0;33m[warning] ",
    [INK_LOG_ERROR] = "\033[0;31m[error] "
};

void inkdevBuildLog(void* logger, const char* msg, const char* file, u32 line, ink_log_type type) {
    printf("%s%s\n", buildLogPrefix[type], msg);
}

struct {
    gameDLL dll;

    u32 currBuildId;

    usize stateSize;
    void* state; 
    bool needReload;
} gameState;

void reload(const char* dllPath) {

    bool success = true;
    gameDLL newDLL = loadDLL(dllPath, &success);

    if(!success) {
        dropDLL(&newDLL);
        return;
    }

    void* newState = ink_malloc(newDLL.stateTypeInfo->size);
    ink_transferData(gameState.state, gameState.dll.stateTypeInfo, newState, newDLL.stateTypeInfo);

    ink_free(gameState.state, gameState.stateSize);
    gameState.state = newState;
    gameState.stateSize = newDLL.stateTypeInfo->size;

    dropDLL(&gameState.dll);
    gameState.dll = newDLL;

    if(gameState.dll.reload)
        gameState.dll.reload(gameState.state);

}

#include <inkling/window.h>
#include <inkling/math.h>
#include <inkling/gfx.h>

ink_arena compilationArena;

int main(int argc, char** argv) {

    ink_logger logger = (ink_logger){
        .log = inkdevBuildLog
    };
    ink_setLogger(&logger);

    compilationArena = ink_makeArena(1024 * 1024);

    if(argc == 2 && strcmp(argv[1], "build") == 0) {
        build(&compilationArena);
        return 0;
    }

    ink_initWindow();

    bool success = true;
    ink_str dllPath = rebuild(&compilationArena, 0, &success);
    if(!success)
        return -1;
    gameState.dll = loadDLL(dllPath.str, &success);
    if(!success)
        return -1;

    gameState.state = ink_malloc(gameState.dll.stateTypeInfo->size);
    gameState.dll.init(gameState.state);
    
    gameState.currBuildId = 1;
    ink_arenaClear(&compilationArena);

    while(ink_continueGameLoop()) {
        f32 dt = ink_winBeginFrame(); 

        gameState.dll.update(dt, gameState.state);

        if(ink_keyPressed(INK_KEY_F1)) {
            bool success = true;
            ink_str dllPath = rebuild(&compilationArena, gameState.currBuildId, &success);
            if(success) {
                reload(dllPath.str);
                gameState.currBuildId++;
            }
            ink_arenaClear(&compilationArena);
        }

        ink_winEndFrame();
    }
    
}
