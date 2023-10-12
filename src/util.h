#ifndef UTIL_H
#define UTIL_H

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>


#define MAX(a, b) a > b ? a : b
#define MIN(a, b) a > b ? b : a


// This is just a placeholder function, I plan on making a more advanced error handling system later
static inline void die(const char *msg) {
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

static inline void framebuffer_cb(GLFWwindow *win, int width, int height) {
    glViewport(0, 0, width, height);
}

static inline void winsize_cb(GLFWwindow *win, int width, int height) {
    glfwSetWindowSize(win, width, height);
}

static inline void winpos_cb(GLFWwindow *win, int x, int y) {
    glfwSetWindowPos(win, x, y);
}

#endif