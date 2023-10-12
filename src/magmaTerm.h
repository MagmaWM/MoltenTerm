#ifndef SOMETERM_H
#define SOMETERM_H

#include "window.h"

typedef struct Terminal {
    int main_fd, worker_fd;
    char *current_proc;
    struct termios *attrs;
    int rows, columns;
    char *current_working_dir;

} Terminal;

typedef struct MagmaTerm {
    int running;
    Window *window;
    Terminal *term;
    void (*on_someterm_fatal)(const char *description);
    void (*on_someterm_error)(const char *description);
} MagmaTerm;


void cleanup(MagmaTerm *magma);
void close_terms(MagmaTerm *magma);
// Initialize glfw, load opengl, create a heap-allocated MagmaTerm struct. 
MagmaTerm *initialize_magmaterm();
int load_opengl(MagmaTerm *magma);
Terminal *new_terminal(MagmaTerm *magma);
int pipe_to_shell_proc(Terminal *term, int worker_fd);
void run(MagmaTerm *magma);



#endif