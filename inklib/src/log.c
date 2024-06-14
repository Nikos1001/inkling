
#include <inkling/common.h>
#include <inkling/log.h>
#include <inkling/marks.h>
#include <stdarg.h>

const char* ink_logTypeNames[] = {
    [INK_LOG_INFO] = "info",
    [INK_LOG_WARNING] = "warning",
    [INK_LOG_ERROR] = "error"
};

#include <stdio.h>

void fallback_log(void* logger, const char* msg, const char* file, u32 line, ink_log_type type) {
    printf("WARNING: FALLBACK LOGGER USED\n");
    printf("%s:%d\n", file, line);
    printf("[%s] %s\n", ink_logTypeNames[type], msg);
}

ink_logger fallback_logger = (ink_logger){
    .log = fallback_log
};

ink_threadLocal ink_logger* current_logger = &fallback_logger;

void ink_setLogger(ink_logger* logger) {
    current_logger = logger;
}

ink_threadLocal char logBuffer[4096];

void ink_log(const char* msg, const char* file, u32 line, ink_log_type type, ...) {
    va_list args;
    va_start(args, type);
    vsnprintf(logBuffer, sizeof(logBuffer), msg, args);
    va_end(args);
    current_logger->log(current_logger, logBuffer, file, line, type);
}
