
#ifndef INKLING_LOG_H
#define INKLING_LOG_H

#include <inkling/types.h>

typedef enum {
    INK_LOG_INFO,
    INK_LOG_WARNING,
    INK_LOG_ERROR
} ink_log_type;

extern const char* ink_logTypeNames[];

typedef struct {
    void (*log)(void* logger, const char* msg, const char* file, u32 line, ink_log_type type);
} ink_logger;

void ink_setLogger(ink_logger* logger);
void ink_log(const char* msg, const char* file, u32 line, ink_log_type type, ...);

#define ink_info(msg, ...) ink_log(msg, __FILE__, __LINE__, INK_LOG_INFO, ##__VA_ARGS__) 
#define ink_warning(msg, ...) ink_log(msg, __FILE__, __LINE__, INK_LOG_WARNING, ##__VA_ARGS__) 
#define ink_error(msg, ...) ink_log(msg, __FILE__, __LINE__, INK_LOG_ERROR, ##__VA_ARGS__) 

#endif
