
#include <stdio.h>
#include <inkling/common.h>
#include <inkling/log.h>
#include <inkling/file.h>
#include <sys/stat.h>
#include <pthread.h>
#include <dlfcn.h>

#define INKLIB_INSTALL_PATH ".inkling"
#define INKLIB_INCLUDE_PATH INKLIB_INSTALL_PATH "/include"
#define INKLIB_LIB_PATH INKLIB_INSTALL_PATH "/lib"

#include "dll.h"

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

ink_str rebuild(ink_arena* compilationArena) {

    ink_path cwd = ink_cwd(compilationArena);

    const char* homeDir = getenv("HOME");

    ink_strBuilder buildPathBuilder = ink_makeStrBuilder();
    ink_writeFmt(compilationArena, &buildPathBuilder, ".build/g%d.so", gameState.currBuildId);
    gameState.currBuildId++;

    ink_str buildPath = ink_getBuiltStr(&buildPathBuilder);

    ink_strBuilder commandBuilder = ink_makeStrBuilder();
    ink_writeFmt(compilationArena, &commandBuilder, "clang -shared -o %s ", buildPath.str);
    ink_writeStr(compilationArena, &commandBuilder, "-I"); 
    ink_writeStr(compilationArena, &commandBuilder, homeDir);
    ink_writeStr(compilationArena, &commandBuilder, "/" INKLIB_INCLUDE_PATH " -L");
    ink_writeStr(compilationArena, &commandBuilder, homeDir);
    ink_writeStr(compilationArena, &commandBuilder, "/" INKLIB_LIB_PATH " -linklib-dev ");

    for(ink_pathList* file = ink_walkDir(compilationArena, &cwd); file != NULL; file = file->next) {
        ink_str ext = ink_pathExt(&file->path);
        if(ink_strEq(&ext, "c")) {
            const char* relPath = file->path.path.str + file->path.pieces[cwd.nPieces].start;
            ink_writeStr(compilationArena, &commandBuilder, relPath);
            ink_writeChar(compilationArena, &commandBuilder, ' ');
        }
    }

    ink_str command = ink_getBuiltStr(&commandBuilder); 

    ink_info("Rebuilding...");
    ink_info(command.str);

    system("rm -rf .build");
    system("mkdir -p .build");
    system(command.str);

    return buildPath;

}

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
        ink_str dllPath = rebuild(&compilationArena);
        reload(dllPath.str);
        pthread_mutex_unlock(&gameState.mutex);
    }

    ink_dropArena(&compilationArena);
}

#include <inkling/window.h>
#include <inkling/math.h>
#include <inkling/gfx.h>

int main(int argc, char** argv) {

    ink_initWindow();

    gameState.stateSize = 0;
    gameState.state = NULL;
    pthread_mutex_init(&gameState.mutex, NULL);

    ink_logger reloadLogger = (ink_logger){
        .log = inkdevBuildLog
    };
    ink_setLogger(&reloadLogger);

    ink_arena buildArena = ink_makeArena(1024 * 1024);
    ink_str dllPath = rebuild(&buildArena);
    reload(dllPath.str);
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
