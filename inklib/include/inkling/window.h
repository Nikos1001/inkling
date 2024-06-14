
#ifndef INKLING_WINDOW_H
#define INKLING_WINDOW_H

#include <inkling/types.h>

void ink_initWindow();
bool ink_continueGameLoop();
void ink_winEndFrame();

void ink_setWindowTitle(const char* title);
f32 ink_getWindowAspect();



#endif
