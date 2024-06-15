
#include "build.h"
#include <inkling/file.h>

#define INKLIB_INSTALL_PATH ".inkling"
#define INKLIB_INCLUDE_PATH INKLIB_INSTALL_PATH "/include"
#define INKLIB_LIB_PATH INKLIB_INSTALL_PATH "/lib"


void addSourceFilesToBuildCommand(ink_arena* arena, ink_strBuilder* commandBuilder, ink_path* cwd) {
    for(ink_pathList* file = ink_walkDir(arena, cwd); file != NULL; file = file->next) {
        ink_str ext = ink_pathExt(&file->path);
        if(ink_strEq(&ext, "c")) {
            const char* relPath = file->path.path.str + file->path.pieces[cwd->nPieces].start;
            ink_writeStr(arena, commandBuilder, relPath);
            ink_writeChar(arena, commandBuilder, ' ');
        }
    }
}

ink_str rebuild(ink_arena* arena, u32 buildId, bool* success) {
    ink_path cwd = ink_cwd(arena);

    const char* homeDir = getenv("HOME");

    ink_strBuilder buildPathBuilder = ink_makeStrBuilder();
    ink_writeFmt(arena, &buildPathBuilder, ".build/g%d.so", buildId);

    ink_str buildPath = ink_getBuiltStr(&buildPathBuilder);

    ink_strBuilder commandBuilder = ink_makeStrBuilder();
    ink_writeFmt(arena, &commandBuilder, "clang -shared -o %s -I%s/" INKLIB_INCLUDE_PATH " -L%s/" INKLIB_LIB_PATH " -linklib-dev ", buildPath.str, homeDir, homeDir);
    addSourceFilesToBuildCommand(arena, &commandBuilder, &cwd);

    ink_str command = ink_getBuiltStr(&commandBuilder); 

    ink_info("Rebuilding...");
    ink_info(command.str);

    system("mkdir -p .build");
    if(system(command.str) != 0) {
        *success = false;
    }

    return buildPath;
}

#define LINK_ARGS "-framework Cocoa -framework IOKit -framework CoreVideo -framework OpenGL -framework QuartzCore"

void build(ink_arena* arena) {
    ink_path cwd = ink_cwd(arena);

    const char* homeDir = getenv("HOME");

    ink_strBuilder commandBuilder = ink_makeStrBuilder();
    ink_writeFmt(arena, &commandBuilder, "clang -o .build/game -I%s/" INKLIB_INCLUDE_PATH " -L%s/" INKLIB_LIB_PATH " -linkrun -linklib -lglewd -lglfw3 " LINK_ARGS " ", homeDir, homeDir);
    addSourceFilesToBuildCommand(arena, &commandBuilder, &cwd);

    ink_str command = ink_getBuiltStr(&commandBuilder); 
    ink_info("Building...");
    ink_info(command.str);

    system("mkdir -p .build");
    system(command.str);

    ink_info("Build done.");
}
