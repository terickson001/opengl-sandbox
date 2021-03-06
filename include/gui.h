#ifndef _GUI_H_
#define _GUI_H_

#include "glmath2.h"
#include "lib.h"
#include "window.h"
#include "keyboard.h"

#define MAX_ROW_ITEMS 16

typedef enum Gui_Cursor
{
    GUI_CURSOR_ARROW,
    GUI_CURSOR_BAR,
    GUI_CURSOR_HRESIZE,
    GUI_CURSOR_VRESIZE,
    GUI_CURSOR_HAND,
} Gui_Cursor;

typedef enum Gui_Res
{
    GUI_RES_SUBMIT   = 0x1,
    GUI_RES_UPDATE   = 0x2,
    GUI_RES_ACTIVE   = 0x4,
} Gui_Res;

typedef enum Gui_Opt
{
    GUI_OPT_RIGHT      = 0x01,
    GUI_OPT_LEFT       = 0x02,
    GUI_OPT_BORDER     = 0x04,
    GUI_OPT_HOLD_FOCUS = 0x08,
    GUI_OPT_BOTTOM     = 0x10, // RIGHT << 4
    GUI_OPT_TOP        = 0x20, // LEFT  << 4
} Gui_Opt;

typedef enum Gui_Color
{
    GUI_COLOR_BASE = 0,
    GUI_COLOR_BASE_HOVER,
    GUI_COLOR_BASE_FOCUS,
    GUI_COLOR_BUTTON,
    GUI_COLOR_BUTTON_HOVER,
    GUI_COLOR_BUTTON_FOCUS,
    GUI_COLOR_BORDER,
    GUI_COLOR_TEXT,
    GUI_COLOR_MARK,
    GUI_COLOR_TITLE,
    GUI_COLOR_TITLE_TEXT,
    GUI_COLOR_WINDOW,
    GUI_COLOR_CLOSE,
    GUI_COLOR_COUNT,
} Gui_Color;

typedef struct Gui_Style
{
    void *font;
    int text_height;
    Vec2f size; // Default element size
    i32 border_size;
    i32 padding;
    i32 spacing;
    i32 thumb_size;
    i32 title_size;
    Vec4f colors[GUI_COLOR_COUNT];
} Gui_Style;

#pragma GCC diagnostic ignored "-Wmissing-braces"
static const Gui_Style GUI_DEFAULT_STYLE = {
    .font        = 0,
    .text_height = 13,
    .size        = {70, 25},
    .border_size = 1,
    .padding     = 5,
    .spacing     = 2,
    .thumb_size  = 15,
    .title_size  = 25,
    .colors = {
        {30,  30,  30,  255}, // BASE
        {35,  35,  35,  255}, // BASE_HOVER
        {40,  40,  40,  255}, // BASE_FOCUS
        {75,  75,  75,  255}, // BUTTON
        {95,  95,  95,  255}, // BUTTON_HOVER
        {115, 115, 115, 255}, // BUTTON_FOCUS
        {25,  25,  25,  255}, // BORDER
        {230, 230, 230, 255}, // TEXT
        {90,  100, 225, 70 }, // MARK
        {25,  25,  25,  255}, // TITLE
        {240, 240, 240, 255}, // TITLE_TEXT
        {50,  50,  50,  255}, // WINDOW
        {224, 40,  40,  255}, // CLOSE
    },
};

typedef struct Gui_Rect
{
    f32 x, y;
    f32 w, h;
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
    i32 id; // @Note(Tyler): How should I identifiy icons?
    i32 color;
    Gui_Color color_id;
} Gui_Draw_Icon;

typedef struct Gui_Draw
{
    Gui_Draw_Kind kind;
    i32 layer;
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

typedef struct Gui_Text_State
{
    i32 cursor;
    i32 mark;
    f64 cursor_last_updated;
    // @Note(Tyler): Character offset for one-line text box
    //               Line offset for multi-line text box (Eventually)
    i32 offset;
} Gui_Text_State;

typedef struct Gui_Window
{
    char *title;
    Gui_Rect rect;
    Gui_Rect container;
    b32 open;
    i32 layer;
    struct
    {
        i32 start;
        i32 end;
    } draws;
} Gui_Window;
    
typedef struct Gui_Context
{
    u64 hover;
    u64 focus;
    i32 layer;
    
    Array(Gui_Rect) containers;
    Gui_Layout layout;
    Gui_Style style;
    
    Array(Gui_Draw) draws;
    i32 draw_index;
    Gui_Cursor cursor_icon;

    // Inputs
    Vec2f cursor;
    Vec2f last_cursor;
    Vec2f scroll;
    KeyState mouse[3];
    char text_input[128];
    f64 time;

    // Window State
    Gui_Window *window_hover;
    Gui_Window *window_focus;
    i32 win_top_layer;
    Array(Gui_Window *) windows;
    
    Gui_Text_State text_box;
    
    float (*get_char_width)(void *font, char c, int size);

    char num_input_buf[64];
} Gui_Context;


Gui_Context gui_init();
void gui_input_mouse(Gui_Context *ctx, KeyState *buttons, Vec2f pos, Vec2f scroll);
void gui_begin(Gui_Context *ctx, Window win);
void gui_end(Gui_Context *ctx);

// Elements
void gui_label(Gui_Context *ctx, char *str, u32 opt);
u32 gui_button(Gui_Context *ctx, char *label, i32 icon, u32 opt);
u32 gui_slider(Gui_Context *ctx, char *label, f32 *value, char const *fmt, f32 min, f32 max, f32 step, u32 opt);
u32 gui_text_input(Gui_Context *ctx, char *label, char *buf, int buf_size, u32 opt);
u32 gui_number_input(Gui_Context *ctx, char *label, f32 *value, char const *fmt, f32 min, f32 max, f32 step, u32 opt);
Gui_Window gui_window_init(Gui_Context *ctx, char *title, Gui_Rect rect);
u32 gui_window(Gui_Context *ctx, Gui_Window *win, u32 opt);
void gui_window_end(Gui_Context *ctx);

// Util
b32 gui_hover(Gui_Context *ctx, char *label, int icon);
b32 gui_active(Gui_Context *ctx, char *label, int icon);

// Draw
void gui_draw_rect(Gui_Context *ctx, Gui_Rect rect, u64 id, Gui_Color color, u32 opt);
b32 gui_next_draw(Gui_Context *ctx, Gui_Draw *ret);

// Layout
void gui_row(Gui_Context *ctx, i32 items, i32 *widths, i32 height);

#endif
