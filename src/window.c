#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include "window.h"

Window *new_window() {
    Window *window = calloc(1, sizeof(Window));
    window->title = "Broken";
    window->glfw_window = glfwCreateWindow(650, 650, window->title, NULL, NULL);
    glfwMakeContextCurrent(window->glfw_window);
    return window;
}

void set_bg_color(Window *window, int a, int r, int g, int b) {
    window->bg_col = (a << 24) + (r << 16) + (g << 8) + (b);
    
}

void set_win_fb_callback(Window *window, GLFWframebuffersizefun fun) {
    glfwSetFramebufferSizeCallback(window->glfw_window, fun);
}

void set_win_pos_callback(Window *window, GLFWwindowposfun fun) {
    glfwSetWindowPosCallback(window->glfw_window, fun);
}

void set_win_size_callback(Window *window, GLFWwindowsizefun fun) {
    glfwSetWindowSizeCallback(window->glfw_window, fun);
}
