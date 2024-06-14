
#include <inkling/input.h>

#include <GLFW/glfw3.h>

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
