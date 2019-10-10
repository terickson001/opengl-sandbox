#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "window.h"
#include "lib.h"
#include "glmath2.h"

typedef enum KeyState
{
    KeyState_NONE = 0,
    KeyState_PRESSED,
    KeyState_DOWN,
    KeyState_RELEASED,
    KeyState_UP
} KeyState;

void update_keystate(GLFWwindow *window, int keycode, int scancode, int action, int mods);
void keyboard_char_callback(GLFWwindow *window, u32 codepoint);
KeyState get_keystate(int k);
b32 key_down(int k);
b32 key_pressed(int k);
b32 key_released(int k);
void keyboard_text_hook(char *text_buffer);
void keyboard_text_unhook();


void update_mousepos(GLFWwindow *window, double x, double y);
void update_mousestate(GLFWwindow *window, int button, int action, int mods);
KeyState get_mousestate(int m);
b32 mouse_down(int m);
b32 mouse_pressed(int m);
b32 mouse_released(int m);
Vec2f mouse_pos();
void mouse_set_pos(Vec2f pos);

#endif // _KEYBOARD_H
