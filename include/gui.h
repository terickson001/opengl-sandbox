#ifndef _GUI_H_
#define _GUI_H_

#include "glmath2.h"
#include "lib.h"
#include "window.h"
#include "keyboard.h"

#define MAX_ROW_ITEMS 16

typedef enum Gui_Opt
{
    GUI_OPT_BORDER = 0x1,
    GUI_OPT_CENTER = 0x2,
    GUI_OPT_RIGHT  = 0x4,
    GUI_OPT_HOLD_FOCUS = 0x8,
} Gui_Opt;

typedef enum Gui_Color
{
    GUI_COLOR_BUTTON = 0,
    GUI_COLOR_BUTTON_HOVER,
    GUI_COLOR_BUTTON_FOCUS,
    GUI_COLOR_BORDER,
    GUI_COLOR_TEXT,
    GUI_COLOR_COUNT,
} Gui_Color;

typedef struct Gui_Style
{
    Vec2f size; // Default element size
    i32 border_size;
    Vec4f colors[GUI_COLOR_COUNT];
} Gui_Style;

#pragma GCC diagnostic ignored "-Wmissing-braces"
static const Gui_Style GUI_DEFAULT_STYLE = {
    {70, 15},
    1,
    {
        {75,  75,  75,  255}, // BUTTON
        {95,  95,  95,  255}, // BUTTON_HOVER
        {115, 115, 115, 255}, // BUTTON_FOCUS
        {25,  25,  25,  255}, // BORDER
        {230, 230, 230, 255}, // TEXT
    },
};

typedef struct Gui_Rect
{
    i32 x, y;
    i32 w, h;
} Gui_Rect;

typedef enum Gui_Draw_Kind
{
    GUI_DRAW_RECT = 1,
    GUI_DRAW_TEXT,
    GUI_DRAW_ICON,
} Gui_Draw_Kind;

typedef struct Gui_Draw_Rect
{
    Gui_Rect rect;
    Vec4f color;
    Gui_Color color_id;
} Gui_Draw_Rect;

typedef struct Gui_Draw_Text
{
    Vec2f pos;
    f32 size;
    Vec4f color;
    Gui_Color color_id;
    char *text;
} Gui_Draw_Text;

typedef struct Gui_Draw_Icon
{
    Gui_Rect rect;
    i32 id; // How should I identifiy icons?
    i32 color;
    Gui_Color color_id;
} Gui_Draw_Icon;

typedef struct Gui_Draw
{
    Gui_Draw_Kind kind;
    b32 focus;
    b32 hover;
    union
    {
        Gui_Draw_Rect rect;
        Gui_Draw_Text text;
        Gui_Draw_Icon icon;
    };
} Gui_Draw;

typedef struct Gui_Layout
{
    Vec2f pos;
    Vec2f size;
    i32 items;
    i32 widths[MAX_ROW_ITEMS];
    i32 curr_item;
} Gui_Layout;

typedef struct Gui_Context
{
    u64 hover;
    u64 focus;
    Gui_Layout layout;
    Gui_Style style;
    Array(Gui_Draw) draws;
    i32 draw_index;
    Vec2f cursor;
    KeyState mouse[3];
} Gui_Context;


Gui_Context gui_init();
void gui_input_mouse(Gui_Context *ctx, KeyState *buttons, Vec2f pos);
void gui_begin(Gui_Context *ctx, Window win);
void gui_end(Gui_Context *ctx);

b32 gui_button(Gui_Context *ctx, char *label, i32 icon, i32 opt);

// Util
b32 gui_hover(Gui_Context *ctx, char *label, int icon);
b32 gui_active(Gui_Context *ctx, char *label, int icon);
b32 gui_button_rect(Gui_Context *ctx, char *label, int icon, int opt);

// Draw
void gui_draw_rect(Gui_Context *ctx, Gui_Rect rect, u64 id, Gui_Color color, i32 opt);
b32 gui_next_draw(Gui_Context *ctx, Gui_Draw *Ret);

// Layout
void gui_row(Gui_Context *ctx, i32 items, i32 *widths, i32 height);

#endif
