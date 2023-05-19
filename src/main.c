#define _XOPEN_SOURCE 600

#include "moltenterm.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>


MoltenTerm *new_molten_context() {
    MoltenTerm *term = (MoltenTerm *) malloc(sizeof(MoltenTerm));
    if (term == NULL) {
        perror("Failure creating molten context pointer");
        return NULL;
    }
    
    return term;
}

int open_pty(MoltenTerm *molten) {
    if (molten == NULL) {
        // No perror call because errno isn't set by any function prior to this if statement 
        // (unless a function prior to this if statement that does set errno failed, but in that case, 
        // it wouldn't have anything to do with the null MoltenTerm pointer in the 'molten' arg)
        fprintf(stderr, "Molten context pointer is null, cannot create pt file descriptors");
        return 0;
    }
    int master_fd = posix_openpt(O_RDWR | O_NOCTTY);
    if (master_fd == -1) {
        perror("Failure creating master pt descriptor");
        return 0;
    }

    if (grantpt(master_fd) == -1) {
        perror("Failed to set owner of slave pt to the master file descriptor");
        return 0;
    }

    if (unlockpt(master_fd) == -1) {
        perror("Unable to unlock slave pt");
        return 0;
    }

    char *slave_path = ptsname(master_fd);
    if (slave_path == NULL) {
        perror("Failure getting slave pt path");
        return 0;
    }
    
    int slave_fd = open(slave_path, O_RDWR | O_NOCTTY);
    if (slave_fd == -1) {
        perror("Failure opening slave pt");
        return 0;
    }

    molten->master_pt = master_fd;
    molten->slave_pt = slave_fd;
    return 1;
}

int new_window(MoltenTerm *molten) {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize glfw\n");
        return 0;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "MagmaTerm", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to create glfw window\n");
        glfwTerminate();
        return 0;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGL()) {
        fprintf(stderr, "Failed to load opengl\n");
        glfwTerminate();
        return 0;
    }
    glfwSetFramebufferSizeCallback(window, on_window_fb_size_changed);
    molten->window = window;
    return 1;
}

void on_window_fb_size_changed(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    MoltenTerm *term = new_molten_context();
    if (term == NULL) {
        return 1;
    }
    if (!open_pty(term)) {
        return 1;
    }
    if (!new_window(term)) {
        return 1;
    }
    printf("Master pt descriptor: %d\nSlave pt descriptor: %d\n", term->master_pt, term->slave_pt);

    while(!glfwWindowShouldClose(term->window)) {
        glClearColor(0.7f, 0.1f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(term->window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
