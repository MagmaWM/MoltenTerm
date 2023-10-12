#define _XOPEN_SOURCE 600
#include "magmaTerm.h"
#include "window.h"
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <termios.h>
#include <termio.h>
#include <unistd.h>
#include "util.h"
#include <string.h>
#include <fcntl.h>
#include <pwd.h>
#include <signal.h>

// This is a global variable of the terminal runtime struct
// In order to maintain simple control flow and prevent any data races, DO NOT USE THIS VARIABLE UNLESS ABSOLUTELY NECESSARY!!
// I only made this so I could be able to use it in the event of the program exitting before the run loop, and it shouldn't be used 
// for anymore purposes.
static MagmaTerm *instance;

char *get_login_shell();
void handle_signal(int signum);
void on_glfw_error(int code, const char *description);
void on_premature_exit();


void cleanup(MagmaTerm *magma) {
    if (magma != NULL) {
        if (magma->window != NULL) {
            if (magma->window->glfw_window != NULL)
                glfwDestroyWindow(magma->window->glfw_window);
            free(magma->window);
        }
        if (magma->term != NULL) {
            // Comparing the pty fds to the std{in,out,err} fds in order to avoid closing the std{in,out,err} fds
            close_terms(magma);
            free(magma->term);
        }
        free(magma);
    }
}

void close_terms(MagmaTerm *magma) {
    // Comparing the pty fds to the std{in,out,err} fds in order to avoid closing the std{in,out,err} fds
    if (magma->term->main_fd > MAX(STDIN_FILENO, MAX(STDERR_FILENO, STDOUT_FILENO))) {
        close(magma->term->main_fd);
        close(magma->term->worker_fd);
    }
}

char *get_login_shell() {
    struct passwd *pwd;
    pwd = getpwuid(getuid());
    if (pwd == NULL) {
        // Maybe default to /bin/sh
        die("Unable to get user's login shell");
        return NULL;
    }
    if (pwd->pw_shell == NULL) {
        die("Unable to get user's login shell");
        return NULL;
    }

    setenv("SHELL", pwd->pw_shell, 1);
    setenv("HOME", pwd->pw_dir, 1);
    setenv("USER", pwd->pw_name, 1);
    return pwd->pw_shell;
}

void handle_signal(int signum) {
    if (signum == SIGINT || signum == SIGABRT) {
        printf("Caught SIGINT or SIGABRT\n");
        exit(1);
    }
}

MagmaTerm *initialize_magmaterm() {
    MagmaTerm *magma = calloc(1, sizeof(MagmaTerm));
    instance = magma;
    atexit(on_premature_exit);
    struct sigaction sig_act = {
        .sa_handler = handle_signal
    };
    sigaction(SIGINT, &sig_act, NULL);
    sigaction(SIGABRT, &sig_act, NULL);
    magma->running = 1;
    magma->on_someterm_fatal = die;
    magma->on_someterm_error = die;
    glfwSetErrorCallback(on_glfw_error);
    if(!glfwInit()) {
        magma->on_someterm_fatal("Glfw initialization");
        return NULL;
    }
    Window *window = new_window();
    magma->window = window;
    // The title will change once the user's login shell is executed
    load_opengl(magma);
    set_win_fb_callback(window, framebuffer_cb);
    set_win_size_callback(window, winsize_cb);
    set_win_pos_callback(window, winsize_cb);
    return magma;
}

int load_opengl(MagmaTerm *magma) {
    if (!gladLoaderLoadGL()) {
        magma->on_someterm_fatal("Unable to load opengl");
        return -1;
    }
    return 0;
}

Terminal *new_terminal(MagmaTerm *magma) {
    Terminal *newterm = calloc(1, sizeof(Terminal));
    int main_fd = posix_openpt(O_RDWR | O_NONBLOCK);
    // I am so sorry you had to see what is below
    if (main_fd < 0) {
        perror("Pty main open");
        magma->on_someterm_fatal("Unable to open pty files");
        return NULL;
    }
    if ((grantpt(main_fd)) < 0) {
        perror("Pty main grant");
        magma->on_someterm_fatal("Unable to grant pty");
        return NULL;
    }
    if ((unlockpt(main_fd)) < 0) {
        perror("Unlock pt");
        magma->on_someterm_fatal("Unable to unlock pty");
        return NULL;
    }
    char *worker_path = ptsname(main_fd);
    printf("hello4\n");
    if (worker_path == NULL) {
        perror("Worker pt path");
        magma->on_someterm_fatal("Unable to get path of worker pt");
        return NULL;
    }
    int worker_fd = open(worker_path, O_RDWR | O_NONBLOCK);
    if (worker_fd < 0) {
        perror("Pty worker open");
        magma->on_someterm_fatal("Unable to open pty files");
        return NULL;
    }
    newterm->main_fd = main_fd;
    newterm->worker_fd = worker_fd;
    pipe_to_shell_proc(newterm, worker_fd);
    return newterm;
}

void on_glfw_error(int code, const char *description) {
    fprintf(stderr, "GLFW Error %d: %s\n", code, description);
}

void on_premature_exit() {
    cleanup(instance);
}

// NOTE TO CONTRIBUTORS: I know this parameter name is not technically correct in terms of pty anatomy, but please 
// do not change the name of the `worker_fd` param
int pipe_to_shell_proc(Terminal *term, int worker_fd) {
    int worker_p[2];
    if (pipe(worker_p) < 0) {
        perror("pipe");
        return -1;
    }
    switch (fork()) {
        // Error
        case -1:
            perror("Fork");
            die("Unable to fork");
            break;
        // Child process
        case 0: 
            close(worker_p[1]);
            int cmdfd;
            read(worker_p[0], &cmdfd, sizeof(int));
            close(worker_p[0]);
            if (cmdfd < 0) {
                die("Unable to pipe worker fd or fd doesn't exist");
            }
            setsid();

            dup2(cmdfd, STDIN_FILENO);
            dup2(cmdfd, STDOUT_FILENO);
            dup2(cmdfd, STDERR_FILENO);
            if (ioctl(cmdfd, TIOCSCTTY, 0) < 0) {
                perror("set as controlling terminal");
                die("");
            }
            if (cmdfd > STDERR_FILENO) {
                close(cmdfd);
            }
            char *shell = get_login_shell();
            char *args[] = {"", NULL};
            signal(SIGCHLD, SIG_DFL);
            signal(SIGQUIT, SIG_DFL);
            signal(SIGHUP, SIG_DFL);
            signal(SIGTERM, SIG_DFL);
            signal(SIGINT, SIG_DFL);
            signal(SIGALRM, SIG_DFL);

            execvp(shell, args);
            exit(1);
            break;
        default:
            close(worker_p[0]);
            write(worker_p[1], &worker_fd, sizeof(int));
            close(worker_p[1]);
            break;
    }
    return 0;
}

// TEST FUNCTION
void get_input(char **input) {
    int buff_size = sizeof(char);
    char *buff = malloc(buff_size);
    
    while ( (*(buff + (buff_size - 1)) = getchar()) != '\n') {
        buff_size++;
        buff = realloc(buff, buff_size);
    };
    *(buff + buff_size) = '\0';
    *input = buff;
}

void run(MagmaTerm *magma) {
    printf("trace\n");
    while (!glfwWindowShouldClose(magma->window->glfw_window) && magma->running) {
        // Normalized colors
        float a = ((0b11111111 << 24 & magma->window->bg_col) >> 24) / 255.0;
        float r = ((0b11111111 << 16 & magma->window->bg_col) >> 16) / 255.0;
        float g = ((0b11111111 << 8 & magma->window->bg_col) >> 8) / 255.0;
        
        float b = (0b11111111 & magma->window->bg_col) / 255.0;

        glClearColor(r, g, b, a);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(magma->window->glfw_window);
        glfwPollEvents();

        // Test
        char *input;
        get_input(&input);
        write(magma->term->main_fd, input, strlen(input));
        free(input);
    }
}

