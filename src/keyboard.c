#include "keyboard.h"

#include <stdio.h>
#include <GLFW/glfw3.h>

typedef struct Keyboard
{
    KeyState keys[316];
    char *text_buffer;
} Keyboard;
static Keyboard KEYBOARD = {0};

void update_keystate(GLFWwindow *window, int keycode, int scancode, int action, int mods)
{
    int code = keycode - 32;
    if (code > 316)
    {
        fprintf(stderr, "Keycode '%d' out of range\n", keycode);
        return;
    }

    if (action == GLFW_PRESS)
        KEYBOARD.keys[code] = KeyState_PRESSED;
    else if (action == GLFW_RELEASE)
        KEYBOARD.keys[code] = KeyState_RELEASED;
    else if (action == GLFW_REPEAT)
        KEYBOARD.keys[code] = KeyState_REPEAT;
}

KeyState get_keystate(int k)
{
    int code = k - 32;
    if (code > 316)
    {
        fprintf(stderr, "Keycode '%d' out of range\n", k);
        return KeyState_NONE;
    }

    KeyState ret = KEYBOARD.keys[code];
    if (ret == KeyState_PRESSED || ret == KeyState_REPEAT)
        KEYBOARD.keys[code] = KeyState_DOWN;
    else if (ret == KeyState_RELEASED)
        KEYBOARD.keys[code] = KeyState_UP;
    
    return ret;
}

b32 key_down(int k)
{
    KeyState state = get_keystate(k);
    if (KeyState_PRESSED <= state && state <= KeyState_REPEAT)
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

b32 key_repeat(int k)
{
    KeyState state = get_keystate(k);
    if (state == KeyState_PRESSED || state == KeyState_REPEAT)
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

void keyboard_text_hook(char *text_buffer)
{
    KEYBOARD.text_buffer = text_buffer;
}

void keyboard_text_unhook()
{
    KEYBOARD.text_buffer = 0;
}

void keyboard_char_callback(GLFWwindow *window, u32 codepoint)
{
    if (KEYBOARD.text_buffer)
        KEYBOARD.text_buffer[strlen(KEYBOARD.text_buffer)] = (char)codepoint;
}

// Mouse
typedef struct Mouse
{
    KeyState buttons[8];
    Vec2f pos;
    Vec2f scroll;
} Mouse;
static Mouse MOUSE = {0};

void update_mousepos(GLFWwindow *window, double x, double y)
{
    MOUSE.pos.x = x;
    MOUSE.pos.y = y;
}

void update_mousescroll(GLFWwindow *window, double xoff, double yoff)
{
    MOUSE.scroll.x += xoff;
    MOUSE.scroll.y += yoff;
}

void update_mousestate(GLFWwindow *window, int button, int action, int mods)
{
    if (button > 7)
    {
        fprintf(stderr, "Mouse button '%d' out of range\n", button);
        return;
    }

    if (action == GLFW_PRESS)
        MOUSE.buttons[button] = KeyState_PRESSED;
    else if (action == GLFW_RELEASE)
        MOUSE.buttons[button] = KeyState_RELEASED;
}

KeyState get_mousestate(int m)
{
    if (m > 7)
    {
        fprintf(stderr, "Mouse button '%d' out of range\n", m);
        return KeyState_NONE;
    }

    KeyState ret = MOUSE.buttons[m];
    if (ret == KeyState_PRESSED)
        MOUSE.buttons[m] = KeyState_DOWN;
    else if (ret == KeyState_RELEASED)
        MOUSE.buttons[m] = KeyState_UP;
    
    return ret;
}

b32 mouse_down(int m)
{
    KeyState state = get_mousestate(m);
    if (state == KeyState_PRESSED || state == KeyState_DOWN)
        return true;
    
    MOUSE.buttons[m] = state;
    return false;
}

b32 mouse_pressed(int m)
{
    KeyState state = get_mousestate(m);
    if (state == KeyState_PRESSED)
        return true;
    
    MOUSE.buttons[m] = state;
    return false;
}

b32 mouse_released(int m)
{
    KeyState state = get_mousestate(m);
    if (state == KeyState_RELEASED)
        return true;
    
    MOUSE.buttons[m] = state;
    return false;
}

Vec2f mouse_pos()
{
    return MOUSE.pos;
}

void mouse_set_pos(Vec2f pos)
{
    MOUSE.pos = pos;
}

Vec2f mouse_scroll()
{
    Vec2f ret = MOUSE.scroll;
    MOUSE.scroll = (Vec2f){0};
    return ret;
}
