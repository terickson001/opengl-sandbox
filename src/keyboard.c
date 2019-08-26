#include "keyboard.h"

#include <stdio.h>
#include <GLFW/glfw3.h>

void update_keystate(GLFWwindow *window, int keycode, int scancode, int action, int mods)
{
    int code = keycode - 32;
    if (code > 312)
    {
        fprintf(stderr, "Keycode '%d' out of range\n", keycode);
        return;
    }

    if (action == GLFW_PRESS)
        KEYBOARD.keys[code] = KeyState_PRESSED;
    else if (action == GLFW_RELEASE)
        KEYBOARD.keys[code] = KeyState_RELEASED;
}

KeyState get_keystate(int k)
{
    int code = k - 32;
    if (code > 312)
    {
        fprintf(stderr, "Keycode '%d' out of range\n", k);
        return KeyState_NONE;
    }

    KeyState ret = KEYBOARD.keys[code];
    if (ret == KeyState_PRESSED)
        KEYBOARD.keys[code] = KeyState_DOWN;
    else if (ret == KeyState_RELEASED)
        KEYBOARD.keys[code] = KeyState_UP;
    
    return ret;
}

b32 key_down(int k)
{
    int code = k-32;
    return KEYBOARD.keys[code] == KeyState_PRESSED || KEYBOARD.keys[code] == KeyState_DOWN;
}
