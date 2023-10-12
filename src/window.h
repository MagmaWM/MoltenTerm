#ifndef WINDOW_H
#define WINDOW_H

#include <glad/gl.h>
#include <GLFW/glfw3.h>

typedef struct Window {
    GLFWwindow *glfw_window;
    // Glyph *text; Meant to be an array of the font renderings of terminal text content, both input and output. Damn, I gotta learn freetype :(
    char *title;
    // One color for every eight bits. Stored in little endian order in ARGB format, from 0 to 255 (inclusive)
    uint32_t bg_col;
    
} Window;

Window *new_window();
void set_bg_color(Window *window, int a, int r, int g, int b);
void set_win_fb_callback(Window *window, GLFWframebuffersizefun fun);
void set_win_pos_callback(Window *window, GLFWwindowposfun fun);
void set_win_size_callback(Window *window, GLFWwindowsizefun fun);

#endif