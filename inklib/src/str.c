
#include <inkling/str.h>

ink_str ink_makeStr(const char* str) {
    return (ink_str) {
        .len = strlen(str),
        .str = str
    };
}

ink_str ink_copyStr(ink_arena* arena, const char* str) {
    usize len = strlen(str);
    void* strAlloc = ink_arenaAlloc(arena, len + 1);
    strcpy(strAlloc, str);
    return (ink_str) {
        .len = len,
        .str = strAlloc
    };
}

bool ink_strEq(ink_str* str, const char* otherStr) {
    return strcmp(str->str, otherStr) == 0;
}

ink_strBuilder ink_makeStrBuilder() {
    return (ink_strBuilder){
        .str = NULL,
        .len = 0,
        .cap = 0
    };
}

ink_str ink_getBuiltStr(ink_strBuilder* builder) {
    return (ink_str){
        .len = builder->len,
        .str = builder->str
    };
}

void ink_writeChar(ink_arena* arena, ink_strBuilder* builder, char c) {
    builder->len++;
    if(builder->len + 1 > builder->cap) {
        usize newCap = builder->cap == 0 ? 8 : 2 * builder->cap;
        char* newAlloc = ink_arenaAlloc(arena, newCap);
        if(builder->str != NULL)
            strcpy(newAlloc, builder->str);
        builder->cap = newCap;
        builder->str = newAlloc;
    }
    builder->str[builder->len - 1] = c;
    builder->str[builder->len] = '\0';
}

void ink_writeStr(ink_arena* arena, ink_strBuilder* builder, const char* str) {
    while(*str != '\0') {
        ink_writeChar(arena, builder, *str);
        str++;
    }
}

#include <stdio.h>
#include <stdarg.h>

void ink_writeFmt(ink_arena* arena, ink_strBuilder* builder, const char* fmt, ...) {
    // TODO: unscuff
    char buf[4096];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    ink_writeStr(arena, builder, buf);
}
