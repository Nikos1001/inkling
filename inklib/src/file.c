
#include <inkling/file.h>

#include <unistd.h> // getcwd()

#include <dirent.h>

bool ink_isPathSeparator(char c) {
    return c == '/';
}

ink_path ink_joinPiece(ink_arena* arena, ink_path* path, ink_str* piece) {
    char* newPathStr = ink_arenaAlloc(arena, path->path.len + 1 + piece->len + 1);
    memcpy(newPathStr, path->path.str, path->path.len);
    newPathStr[path->path.len] = INK_PATH_SEPARATOR;
    memcpy(newPathStr + path->path.len + 1, piece->str, piece->len + 1);

    ink_pathPiece* pieces = ink_arenaAlloc(arena, (path->nPieces + 1) * sizeof(ink_pathPiece));
    memcpy(pieces, path->pieces, path->nPieces * sizeof(ink_pathPiece));
    pieces[path->nPieces].start = path->path.len + 1;
    pieces[path->nPieces].len = piece->len + 1;

    return (ink_path){
        .path = (ink_str){
            .str = newPathStr,
            .len = path->path.len + 1 + piece->len
        },
        .nPieces = path->nPieces + 1,
        .pieces = pieces
    };
}

ink_str ink_pathExt(ink_path* path) {
    const char* currChar = path->path.str + path->pieces[path->nPieces - 1].start; 
    while(*currChar != '.' && *currChar != '\0') {
        currChar++;
    }
    if(*currChar == '.')
        currChar++;
    return ink_makeStr(currChar);
}

ink_path ink_cwd(ink_arena* arena) {
    char buf[4096];
    getcwd(buf, sizeof(buf));
    usize pathLen = strlen(buf);

    usize nPieces = 1;
    for(usize i = 0; i < pathLen; i++) {
        if(ink_isPathSeparator(buf[i])) {
            nPieces++;
        }
    }

    ink_pathPiece* pieces = ink_arenaAlloc(arena, sizeof(ink_pathPiece) * nPieces);
    char* currChar = buf;
    for(usize i = 0; i < nPieces; i++) {
        char* begin = currChar;
        while(!ink_isPathSeparator(*currChar) && *currChar != '\0')
            currChar++; 
        usize start = begin - buf; 
        usize len = currChar - begin;
        currChar++;
        pieces[i] = (ink_pathPiece){
            .start = start,
            .len = len 
        };
    }

    return (ink_path){
        .path = ink_copyStr(arena, buf),
        .pieces = pieces,
        .nPieces = nPieces
    };
}

void walkDir(ink_arena* arena, ink_path* dirPath, ink_pathList** out) {
    DIR* dir = opendir(dirPath->path.str);
    while(true) {
        struct dirent* entry = readdir(dir);
        if(entry == NULL)
            break;
        if(entry->d_name[0] == '.')
            continue;

        ink_str entryName = ink_makeStr(entry->d_name);
        ink_path filePath = ink_joinPiece(arena, dirPath, &entryName);

        switch(entry->d_type) {
            case DT_REG: {
                ink_pathList* newList = INK_ARENA_ALLOC(arena, ink_pathList);
                newList->path = filePath;
                newList->next = *out;
                *out = newList;
                break;
            }
            case DT_DIR: {
                walkDir(arena, &filePath, out);
                break;
            }
        }
    }
    closedir(dir);
}

ink_pathList* ink_walkDir(ink_arena* arena, ink_path* dir) {
    ink_pathList* fileList = NULL;
    walkDir(arena, dir, &fileList); 
    return fileList;
}
