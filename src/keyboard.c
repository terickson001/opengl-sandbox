#include "keyboard.h"

#include <stdio.h>
#include <GLFW/glfw3.h>

typedef struct Keyboard
{
    KeyState keys[312];
} Keyboard;
static Keyboard KEYBOARD = {0};

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
    KeyState state = get_keystate(k);
    if (state == KeyState_PRESSED || state == KeyState_DOWN)
        return true;
    
    int code = k-32;
    KEYBOARD.keys[code] = state;
    return false;
}

b32 key_pressed(int k)
{
    KeyState state = get_keystate(k);
    if (state == KeyState_PRESSED)
        return true;
    
    int code = k-32;
    KEYBOARD.keys[code] = state;
    return false;
}

b32 key_released(int k)
{
    KeyState state = get_keystate(k);
    if (state == KeyState_RELEASED)
        return true;
    
    int code = k-32;
    KEYBOARD.keys[code] = state;
    return false;
}
