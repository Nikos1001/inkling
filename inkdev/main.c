
#include <stdio.h>
#include <inkling/common.h>
#include <inkling/log.h>
#include <inkling/file.h>
#include <sys/stat.h>
#include <pthread.h>
#include <dlfcn.h>

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
    pthread_mutex_t mutex;
    gameDLL dll;

    u32 currBuildId;

    usize stateSize;
    void* state; 
    bool needReload;
} gameState;

void reload(const char* dllPath) {

    gameDLL newDLL = loadDLL(dllPath);

    if(gameState.state == NULL) {
        if(newDLL.stateTypeInfo != NULL) {
            gameState.state = ink_malloc(newDLL.stateTypeInfo->size);
            gameState.stateSize = newDLL.stateTypeInfo->size;
        } else {
            ink_warning("Game state type information missing, cannot allocate state.");
        }
    } else {
        if(newDLL.stateTypeInfo != NULL) {

            void* newState = ink_malloc(newDLL.stateTypeInfo->size);

            if(gameState.dll.stateTypeInfo == NULL) {
                memcpy(newState, gameState.state, gameState.stateSize);                
                ink_warning("Previous build state type information missing, game state might be corrupted.");
            } else {
                ink_transferData(gameState.state, gameState.dll.stateTypeInfo, newState, newDLL.stateTypeInfo);
            }

            ink_free(gameState.state, gameState.stateSize);
            gameState.state = newState;
            gameState.stateSize = newDLL.stateTypeInfo->size;

        } else {
            ink_free(gameState.state, gameState.stateSize);
            ink_warning("Game state type information missing, cannot transfer state.");

            gameState.state = NULL;
            gameState.stateSize = 0;
        }
    }

    dropDLL(&gameState.dll);
    gameState.dll = newDLL;

    gameState.needReload = true;

}


void* hotReloadThread(void* data) {

    ink_arena compilationArena = ink_makeArena(1024 * 1024);

    ink_logger reloadLogger = (ink_logger){
        .log = inkdevBuildLog
    };
    ink_setLogger(&reloadLogger);

    while(true) {
        getc(stdin);
        pthread_mutex_lock(&gameState.mutex);
        ink_str dllPath = rebuild(&compilationArena, gameState.currBuildId);
        reload(dllPath.str);
        gameState.currBuildId++;
        pthread_mutex_unlock(&gameState.mutex);
    }

    ink_dropArena(&compilationArena);
}

#include <inkling/window.h>
#include <inkling/math.h>
#include <inkling/gfx.h>

int main(int argc, char** argv) {

    ink_logger reloadLogger = (ink_logger){
        .log = inkdevBuildLog
    };
    ink_setLogger(&reloadLogger);

    if(argc == 2 && strcmp(argv[1], "build") == 0) {
        ink_arena buildArena = ink_makeArena(1024 * 1024); 
        build(&buildArena);
        return 0;
    }

    ink_initWindow();

    gameState.stateSize = 0;
    gameState.state = NULL;
    pthread_mutex_init(&gameState.mutex, NULL);

    ink_arena buildArena = ink_makeArena(1024 * 1024);
    ink_str dllPath = rebuild(&buildArena, 0);
    reload(dllPath.str);
    gameState.currBuildId = 1;
    ink_dropArena(&buildArena);

    pthread_t thread;
    pthread_create(&thread, NULL, hotReloadThread, NULL);

    while(ink_continueGameLoop()) {
        f32 dt = ink_winBeginFrame(); 

        pthread_mutex_lock(&gameState.mutex);

        if(gameState.needReload) {
            if(gameState.dll.reload && gameState.state) {
                gameState.dll.reload(gameState.state);
            }
            gameState.needReload = false;
        }

        if(gameState.dll.update) {
            gameState.dll.update(dt, gameState.state);
        }
        pthread_mutex_unlock(&gameState.mutex);

        ink_winEndFrame();
    }
    
}
