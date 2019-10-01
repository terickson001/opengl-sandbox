#ifndef _GUI_H_
#define _GUI_H_

#include "glmath2.h"
#include "lib.h"
#include "texture.h"
#include "sprite.h"
#include "shaders.h"
#include "window.h"

typedef struct Gui_Icon
{
    Sprite *sprite;
} Gui_Icon;

typedef struct Gui_Text
{
    float size;
    char *text;
} Gui_Text;

typedef struct Gui_Box
{
    Sprite *sprite;
} Gui_Box;

typedef struct Gui_Circle
{
    Sprite *sprite;
} Gui_Circle;

typedef enum Gui_Kind
{
    GUI_INVALID,
    GUI_ICON,
    GUI_TEXT,
    GUI_BOX,
    GUI_CIRCLE,
} Gui_Kind;

#define GUI_LEFT     (0x1)
#define GUI_RIGHT    (0x2)
#define GUI_BOTTOM   (0x1)
#define GUI_TOP      (0x2)
#define GUI_CENTER   (0x4)
typedef struct Gui_Anchor
{
    u8 vertical   : 3;
    u8 horizontal : 3;
} Gui_Anchor;

typedef struct Gui_Behavior Gui_Behavior;
typedef struct Gui_Element
{
    Gui_Kind kind;
    Gui_Behavior *behavior;
    Gui_Anchor anchor;
    Vec2f pos;
    Vec2f dims;
    Array(struct Gui_Element *) children;
    union {
        Gui_Icon icon;
        Gui_Text text;
        Gui_Box box;
        Gui_Circle circle;
    };
} Gui_Element;


typedef struct Gui_Button Gui_Button;
typedef i32 (*Gui_Button_Action)(Gui_Behavior *button, Gui_Element *elem, Vec2f cursor_pos);
struct Gui_Button
{
    Gui_Button_Action action;
};

typedef struct Gui_Slider Gui_Slider;
typedef i32 (*Gui_Slider_Action)(Gui_Behavior *slider, Gui_Element *elem, Vec2f cursor_pos);
struct Gui_Slider
{
    float min_value, max_value;
    float step;
    Vec2f values;
    b32 vertical;
    Gui_Slider_Action action;
};

typedef struct Gui_Drag Gui_Drag;
typedef i32 (*Gui_Drag_Action)(Gui_Behavior *drag, Gui_Element *elem, Vec2f cursor_pos);
struct Gui_Drag
{
    Vec2f limits_x;
    Vec2f limits_y;
    Gui_Drag_Action drag;
};

typedef enum Behavior_Kind
{
    GUI_BUTTON = 1,
    GUI_SLIDER,
    GUI_DRAG,
} Behavior_Kind;

typedef struct Gui_Behavior
{
    Behavior_Kind kind;
    union {
        Gui_Button button;
        Gui_Slider slider;
        Gui_Drag drag;
    };
} Gui_Behavior;

typedef struct Gui
{
    Array(Gui_Element *) elements;
    Array(Gui_Element *) active_elements;
    Window win;
} Gui;

void draw_gui(Shader shader, Window win, Gui_Element *element);
Gui_Element gui_box(Sprite *sprite, Vec2f pos, Vec2f dims);
void gui_register_behavior(Gui *g, Gui_Element *elem);

#endif
