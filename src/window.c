#include "window.h"

Window create_window(int width, int height, char *title)
{
    Window win;
    win.width = width;
    win.height = height;
    win.handle = glfwCreateWindow(width, height, title, 0, 0);
    
    return win;
}
