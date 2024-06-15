
#include <inkling/log.h>
#include <inkling/panic.h>
#include <inkling/window.h>
#include <inkling/input.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

GLFWwindow* window = NULL;

extern void keyCallback(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods);

f64 prevFrameTime;

void ink_initWindow() {

    if(window != NULL) {
        ink_warning("Window already initialized.");
        return;
    } 

    if(!glfwInit()) {
        ink_panic("Could not initialize GLFW.");
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(1000, 800, "Inkling", NULL, NULL);
    if(window == NULL) {
        ink_panic("Could not create window.");
    }
    glfwMakeContextCurrent(window);

    glewExperimental = true;
    if(glewInit() != GLEW_OK) {
        ink_panic("Could not initialize GLEW.");
    }

    glfwSetKeyCallback(window, keyCallback);

    prevFrameTime = glfwGetTime();

}

bool ink_continueGameLoop() {
    return !glfwWindowShouldClose(window);
}

f32 ink_winBeginFrame() {
    f64 time = glfwGetTime();
    f32 dt = time - prevFrameTime;
    prevFrameTime = time;
    return dt;
}

void ink_winEndFrame() {
    ink_updateInput();

    glfwPollEvents();
    glfwSwapBuffers(window);
}

void ink_setWindowTitle(const char* title) {
    glfwSetWindowTitle(window, title);
}

f32 ink_getWindowAspect() {
    i32 w, h;
    glfwGetWindowSize(window, &w, &h);
    return (f32)w / (f32)h;
}
