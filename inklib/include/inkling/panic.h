
#ifndef INKLING_PANIC_H
#define INKLING_PANIC_H

#include <stdlib.h>
#include <inkling/log.h>

#define ink_panic(msg) do { ink_error(msg); exit(-1); } while(0)

#endif
