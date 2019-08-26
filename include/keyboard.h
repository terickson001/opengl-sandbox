#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "window.h"
#include "lib.h"
typedef enum KeyState
{
    KeyState_NONE = 0,
    KeyState_PRESSED,
    KeyState_DOWN,
    KeyState_RELEASED,
    KeyState_UP
} KeyState;

typedef struct Keyboard
{
    KeyState keys[312];
} Keyboard;

void update_keystate(GLFWwindow *window, int keycode, int scancode, int action, int mods);
KeyState get_keystate(int k);
b32 key_down(int k);

static Keyboard KEYBOARD = {0};
#endif // _KEYBOARD_H
