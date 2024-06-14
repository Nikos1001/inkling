
#ifndef INKLING_FILE_H
#define INKLING_FILE_H

#include <inkling/str.h>

typedef struct {
    usize start;
    usize len;
} ink_pathPiece;

typedef struct {
    ink_str path;
    ink_pathPiece* pieces;
    usize nPieces; 
} ink_path;

#define INK_PATH_SEPARATOR '/'
bool ink_isPathSeparator(char c);

ink_path ink_joinPiece(ink_arena* arena, ink_path* path, ink_str* piece);
ink_str ink_pathExt(ink_path* path);

ink_path ink_cwd(ink_arena* arena);

typedef struct ink_pathList {
    ink_path path;
    struct ink_pathList* next;
} ink_pathList;

ink_pathList* ink_walkDir(ink_arena* arena, ink_path* dir);

#endif
