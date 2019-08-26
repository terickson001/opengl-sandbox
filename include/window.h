#ifndef _WINDOW_H
#define _WINDOW_H

#include <GLFW/glfw3.h>

typedef struct Window
{
    int width, height;
    GLFWwindow *handle;
} Window;

Window create_window(int width, int height, char *title);

#endif // _WINDOW_H
