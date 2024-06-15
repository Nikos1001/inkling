
#include <inkling/input.h>

#include <GLFW/glfw3.h>

bool prevKeyDown[INK_KEY_COUNT];
bool keyDown[INK_KEY_COUNT];

void keyCallback(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods) {
    if(action == GLFW_PRESS) {
        keyDown[key] = true;
    }
    if(action == GLFW_RELEASE) {
        keyDown[key] = false;
    }
}

bool ink_keyDown(ink_key key) {
    return keyDown[key];
}

bool ink_keyPressed(ink_key key) {
    return keyDown[key] && !prevKeyDown[key];
}

bool ink_keyReleased(ink_key key) {
    return !keyDown[key] && prevKeyDown[key];
}

void ink_initInput() {
    for(ink_key key = 0; key < INK_KEY_COUNT; key++) {
        prevKeyDown[key] = false;
        keyDown[key] = false;
    }
}

void ink_updateInput() {
    memcpy(prevKeyDown, keyDown, sizeof(keyDown)); 
}
