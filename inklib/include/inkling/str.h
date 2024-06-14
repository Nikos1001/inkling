
#ifndef INKLING_STRING_H
#define INKLING_STRING_H

#include <inkling/types.h>
#include <inkling/arena.h>

typedef struct {
    /**
     * Null-terminated
     */
    const char* str;
    /**
     * Number of bytes in string, excluding null byte
     */
    usize len;
} ink_str;

ink_str ink_makeStr(const char* str);
ink_str ink_copyStr(ink_arena* arena, const char* str);
bool ink_strEq(ink_str* str, const char* otherStr);

typedef struct {
    char* str;
    usize len;
    usize cap;
} ink_strBuilder;

ink_strBuilder ink_makeStrBuilder();
ink_str ink_getBuiltStr(ink_strBuilder* builder);
void ink_writeChar(ink_arena* arena, ink_strBuilder* builder, char c);
void ink_writeStr(ink_arena* arena, ink_strBuilder* builder, const char* str);
void ink_writeFmt(ink_arena* arena, ink_strBuilder* builder, const char* fmt, ...);

#endif
