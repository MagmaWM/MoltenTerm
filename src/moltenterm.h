#ifndef MOLTENTERM
#define MOLTENTERM
// Glad always needs to be included before glfw
#include <glad/glad.h>
#include <GLFW/glfw3.h>

typedef struct {
    int master_pt;
    int slave_pt;
    GLFWwindow *window;
} MoltenTerm;

static MoltenTerm *new_molten_context();

/*
    The two functions below return 0 if failed, 1 if succeeded 
*/
static int open_pty(MoltenTerm *molten);
static int new_window(MoltenTerm *molten);
static void on_window_fb_size_changed(GLFWwindow *window, int width, int height);

#endif
