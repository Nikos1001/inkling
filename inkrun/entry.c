
#include <inkling/common.h>
#include <inkling/window.h>

extern ink_typeInfo* stateTypeInfo();
extern void init(void* state);
extern void update(f32 dt, void* state);

int main() {

    ink_initWindow();

    void* state = ink_malloc(stateTypeInfo()->size);
    init(state);

    while(ink_continueGameLoop()) {
        f32 dt = ink_winBeginFrame();
        update(dt, state);
        ink_winEndFrame();
    }

}
