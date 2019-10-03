#include "gui.h"

u64 gui_id(const void *data, isize size)
{
    return hash_crc64(data, size);
}

Gui_Context gui_init()
{
    Gui_Context ctx = {0};
    ctx.style = GUI_DEFAULT_STYLE;
    return ctx;
}

void gui_input_mouse(Gui_Context *ctx, KeyState *buttons, Vec2f pos)
{
    memcpy(ctx->mouse, buttons, sizeof(buttons[0])*3);
    ctx->cursor = pos;
}

b32 gui_mouse_down(Gui_Context *ctx, i32 b)
{
    return ctx->mouse[b] == KeyState_PRESSED || ctx->mouse[b] == KeyState_DOWN;
}

b32 gui_mouse_pressed(Gui_Context *ctx, i32 b)
{
    return ctx->mouse[b] == KeyState_PRESSED;
}

b32 gui_mouse_up(Gui_Context *ctx, i32 b)
{
    return ctx->mouse[b] == KeyState_RELEASED || ctx->mouse[b] == KeyState_UP;
}

b32 gui_mouse_released(Gui_Context *ctx, i32 b)
{
    return ctx->mouse[b] == KeyState_RELEASED;
}

void gui_begin(Gui_Context *ctx, Window win)
{
    if (ctx->draws)
        array_free(ctx->draws);
    array_init(&ctx->draws);
    ctx->draw_index = 0;
    ctx->layout.pos = init_vec2f(0, 0);
    ctx->layout.size.x = win.width;
    ctx->layout.items = 1;
    memset(ctx->layout.widths, 0, sizeof(ctx->layout.widths[0])*MAX_ROW_ITEMS);
    ctx->layout.curr_item = 0;
}

void gui_end(Gui_Context *ctx)
{
    
}

void gui_row(Gui_Context *ctx, i32 items, i32 *widths, i32 height)
{
    ctx->layout.size.y = height;
    ctx->layout.items = items;
    memcpy(ctx->layout.widths, widths, items*sizeof(widths[0]));
    ctx->layout.curr_item = 0;
}

Gui_Rect gui_layout_rect(Gui_Context *ctx)
{
    Gui_Rect rect = {0};

    rect.x = ctx->layout.pos.x;
    rect.y = ctx->layout.pos.y;

    rect.w = ctx->layout.widths[ctx->layout.curr_item];
    rect.h = ctx->layout.size.y;

    if (rect.w <= 0) rect.w += ctx->layout.size.x - rect.x;

    ctx->layout.pos.x += rect.w;
    
    ctx->layout.curr_item++;
    if (ctx->layout.curr_item == ctx->layout.items)
    {
        ctx->layout.pos.x -= ctx->layout.size.x;
        ctx->layout.pos.y += ctx->layout.size.y;
        ctx->layout.curr_item = 0;
    }

    return rect;
}

b32 gui_hover(Gui_Context *ctx, char *label, int icon)
{
    u64 id = label
        ? gui_id(label, strlen(label))
        : gui_id(&icon, sizeof(icon));
    return id == ctx->hover;
}

b32 gui_focus(Gui_Context *ctx, char *label, int icon)
{
    u64 id = label
        ? gui_id(label, strlen(label))
        : gui_id(&icon, sizeof(icon));
    return id == ctx->focus;
}

b32 gui_mouse_over(Gui_Context *ctx, Gui_Rect rect)
{
    Vec2f m = ctx->cursor;
    return (rect.x <= m.x && m.x <= rect.x+rect.w &&
            rect.y <= m.y && m.y <= rect.y+rect.h);
}

void gui_update_focus(Gui_Context *ctx, Gui_Rect rect, u64 id, i32 opt)
{
    b32 mouse_over = gui_mouse_over(ctx, rect);

    if (mouse_over && !mouse_down(0)) ctx->hover = id;

    if (ctx->focus == id)
    {
        if (gui_mouse_pressed(ctx, 0) && !mouse_over) ctx->focus = 0;
        if (!gui_mouse_down(ctx, 0) && ~opt & GUI_OPT_HOLD_FOCUS) ctx->focus = 0;
    }

    if (ctx->hover == id)
    {
        if (!mouse_over) ctx->hover = 0;
        else if (gui_mouse_pressed(ctx, 0)) ctx->focus = id;
    }
    
}

Gui_Draw *gui_add_draw(Gui_Context *ctx, Gui_Draw_Kind kind)
{
    Gui_Draw draw = {0};
    draw.kind = kind;
    array_append(&ctx->draws, draw);
    return &ctx->draws[array_size(ctx->draws)-1];
}

void gui_draw_border(Gui_Context *ctx, Gui_Rect rect, u64 id)
{
    i32 bs = ctx->style.border_size;
    Gui_Color c = GUI_COLOR_BORDER;
    gui_draw_rect(ctx, (Gui_Rect){rect.x,           rect.y,           bs, rect.h}, id, c, 0); // Left
    gui_draw_rect(ctx, (Gui_Rect){rect.x+rect.w-bs, rect.y,           bs, rect.h}, id, c, 0); // Right
    gui_draw_rect(ctx, (Gui_Rect){rect.x+bs,        rect.y-bs,        rect.w-(2*bs), bs}, id, c, 0); // Top
    gui_draw_rect(ctx, (Gui_Rect){rect.x+bs,        rect.y+rect.h-bs, rect.w-(2*bs), bs}, id, c, 0); // Bottom
}

void gui_draw_rect(Gui_Context *ctx, Gui_Rect rect, u64 id, Gui_Color color_id, i32 opt)
{
    if (color_id == GUI_COLOR_BUTTON)
    {
        if      (id == ctx->focus) color_id += 2;
        else if (id == ctx->hover) color_id += 1;
    }
    Vec4f color = ctx->style.colors[color_id];

    Gui_Draw *draw = gui_add_draw(ctx, GUI_DRAW_RECT);
    draw->focus = id == ctx->focus;
    draw->hover = id == ctx->hover;
    draw->rect.rect = rect;
    draw->rect.color = color;
    draw->rect.color_id = color_id;
    
    if (opt & GUI_OPT_BORDER)
        gui_draw_border(ctx, rect, id);
}

b32 gui_next_draw(Gui_Context *ctx, Gui_Draw *ret)
{
    if (!array_size(ctx->draws))
        return false;
    
    *ret = ctx->draws[array_size(ctx->draws)-1];
    array_set_size(&ctx->draws, array_size(ctx->draws)-1);
    return true;
}

b32 gui_button(Gui_Context *ctx, char *label, i32 icon, i32 opt)
{
    u64 id = label
        ? gui_id(label, strlen(label))
        : gui_id(&icon, sizeof(icon));

    b32 res = false;
    Gui_Rect rect = gui_layout_rect(ctx);

    gui_update_focus(ctx, rect, id, 0);
    
    if (id == ctx->focus && mouse_released(0) && id == ctx->hover)
            res = true;

    gui_draw_rect(ctx, rect, id, GUI_COLOR_BUTTON, GUI_OPT_BORDER);
    
    return res;
}
