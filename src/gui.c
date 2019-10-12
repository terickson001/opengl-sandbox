#include "gui.h"

#include "config.h"

// @Robustness(Tyler): Actually support the `opt` parameter correctly

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
    // What should be done here?
    memset(ctx->text_input, 0, 128);
    ctx->layer = 0;
}

void gui_row(Gui_Context *ctx, i32 items, i32 *widths, i32 height)
{
    ctx->layout.size.y = height;
    ctx->layout.items = items;
    memcpy(ctx->layout.widths, widths, items*sizeof(widths[0]));
    ctx->layout.curr_item = 0;
}

Gui_Rect gui_layout_peek_rect(Gui_Context *ctx)
{
    Gui_Rect rect = {0};

    rect.x = ctx->layout.pos.x;
    rect.y = ctx->layout.pos.y;

    rect.w = ctx->layout.widths[ctx->layout.curr_item];
    rect.h = ctx->layout.size.y;

    // Position relative to right for negative values
    if (rect.w <= 0) rect.w += ctx->layout.size.x - rect.x;

    // Adjust for padding
    rect.x += ctx->style.padding;
    rect.y += ctx->style.padding;
    rect.w -= ctx->style.padding * 2;
    rect.h -= ctx->style.padding * 2;
    
    return rect;
}

Gui_Rect gui_layout_rect(Gui_Context *ctx)
{
    Gui_Rect rect = gui_layout_peek_rect(ctx);
    
    // Advance layout position
    ctx->layout.pos.x += rect.w+ctx->style.padding * 2;

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

void gui_update_focus(Gui_Context *ctx, Gui_Rect rect, u64 id, u32 opt)
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
    gui_draw_rect(ctx, (Gui_Rect){rect.x+bs,        rect.y,           rect.w-(2*bs), bs}, id, c, 0); // Top
    gui_draw_rect(ctx, (Gui_Rect){rect.x+bs,        rect.y+rect.h-bs, rect.w-(2*bs), bs}, id, c, 0); // Bottom
}

void gui_draw_text(Gui_Context *ctx, char *str, Gui_Rect rect, Gui_Color color_id, u32 opt)
{
    Vec2f pos = {0};

    float height = ctx->style.text_height;
    float width = ctx->get_text_width(ctx->style.font, str, ctx->style.text_height);

    pos.x = rect.x + (rect.w - width)/2;
    pos.y = rect.y + (rect.h - height)/2;
    
    Gui_Draw *draw = gui_add_draw(ctx, GUI_DRAW_TEXT);
    draw->layer = ctx->layer;
    draw->text.pos = pos;
    draw->text.size = height;
    draw->text.color_id = color_id;
    draw->text.color = ctx->style.colors[color_id];
    
    draw->text.text = malloc(strlen(str)+1);
    strcpy(draw->text.text, str);
}

void gui_draw_rect(Gui_Context *ctx, Gui_Rect rect, u64 id, Gui_Color color_id, u32 opt)
{
    if (color_id == GUI_COLOR_BUTTON || color_id == GUI_COLOR_BASE)
    {
        if      (id == ctx->focus) color_id += 2;
        else if (id == ctx->hover) color_id += 1;
    }
    Vec4f color = ctx->style.colors[color_id];

    i32 base_layer = ctx->layer;
    
    Gui_Draw *draw = gui_add_draw(ctx, GUI_DRAW_RECT);
    draw->focus = id == ctx->focus;
    draw->hover = id == ctx->hover;
    draw->layer = ctx->layer;
    draw->rect.rect = rect;
    draw->rect.color = color;
    draw->rect.color_id = color_id;
    
    ctx->layer = base_layer+1;
    {
        if (opt & GUI_OPT_BORDER)
            gui_draw_border(ctx, rect, id);
    }
    ctx->layer = base_layer;
    
}

b32 gui_next_draw(Gui_Context *ctx, Gui_Draw *ret)
{
    if (ctx->draw_index >= array_size(ctx->draws))
        return false;
    
    *ret = ctx->draws[ctx->draw_index++];
    // array_set_size(&ctx->draws, array_size(ctx->draws)-1);
    return true;
}

void gui_label(Gui_Context *ctx, char *str, u32 opt)
{
    gui_draw_text(ctx, str, gui_layout_rect(ctx), GUI_COLOR_TEXT, opt);
}

u32 gui_button(Gui_Context *ctx, char *label, i32 icon, u32 opt)
{
    u64 id = label
        ? gui_id(label, strlen(label))
        : gui_id(&icon, sizeof(icon));

    b32 res = 0;
    Gui_Rect rect = gui_layout_rect(ctx);

    i32 base_layer = ctx->layer;
    b32 was_focus = ctx->focus == id;
    gui_update_focus(ctx, rect, id, 0);
    
    if (was_focus && gui_mouse_released(ctx, 0) && id == ctx->hover)
            res |= GUI_RES_SUBMIT;

    gui_draw_rect(ctx, rect, id, GUI_COLOR_BUTTON, GUI_OPT_BORDER);
    
    ctx->layer = base_layer+2;
    {
        gui_draw_text(ctx, label, rect, GUI_COLOR_TEXT, 0);
    }
    ctx->layer = base_layer;
    
    return res;
}

u32 gui_slider(Gui_Context *ctx, char *label, f32 *value, char const *fmt, f32 min, f32 max, f32 step, u32 opt)
{
    u64 id = gui_id(label, strlen(label));

    f32 prev = *value;
    Gui_Rect rect = gui_layout_rect(ctx);
    gui_update_focus(ctx, rect, id, opt);

    i32 tw = ctx->style.thumb_size;
    if (ctx->focus == id && gui_mouse_down(ctx, 0))
    {
        *value = min + ((ctx->cursor.x-rect.x-(tw/2.0f))/(rect.w-tw)*(max-min));
        if (step>0) *value = ((i64)((*value + step/2) / step)) * step;
    }

    *value = *value < min
        ? min : *value > max
        ? max : *value;

    u32 res = 0;
    if (*value != prev) res |= GUI_RES_UPDATE; // Value changed

    i32 base_layer = ctx->layer;
    // Draw slider
    gui_draw_rect(ctx, rect, id, GUI_COLOR_BASE, opt | GUI_OPT_BORDER);
    
    // Draw thumb
    ctx->layer = base_layer+2;
    {
        f32 percentage = (*value - min)/(max-min);
        Gui_Rect thumb = {rect.x + percentage * (rect.w-tw), rect.y, tw, rect.h};
        gui_draw_rect(ctx, thumb, id, GUI_COLOR_BUTTON, opt);
    }
    // Draw value
    ctx->layer = base_layer+3;
    {
        char val_buf[128];
        snprintf(val_buf, 128, fmt, *value);
        gui_draw_text(ctx, val_buf, rect, GUI_COLOR_TEXT, 0);
    }
    ctx->layer = base_layer;
    return res;
}

u32 gui_text_input(Gui_Context *ctx, char *label, char *buf, int buf_size, u32 opt)
{
    u64 id = gui_id(label, strlen(label));

    Gui_Rect rect = gui_layout_rect(ctx);
    gui_update_focus(ctx, rect, id, opt | GUI_OPT_HOLD_FOCUS);

    u32 res = 0;

    if (ctx->focus == id)
    {
        // Update textbox
        int len = strlen(buf);
        int in_len = strlen(ctx->text_input);
        int n = buf_size - len - 1 > in_len ? in_len : buf_size - len - 1;
        if (n > 0)
        {
            memcpy(buf + len, ctx->text_input, n);
            len += n;
            buf[len] = 0;
            res |= GUI_RES_UPDATE;
        }

        // Backspace
        if (key_pressed(GLFW_KEY_BACKSPACE) && len > 0)
            buf[--len] = 0;

        // Enter
        if (key_pressed(GLFW_KEY_ENTER))
        {
            ctx->focus = 0;
            res |= GUI_RES_SUBMIT;
        }
    }

    i32 base_layer = ctx->layer;
    // Draw box
    gui_draw_rect(ctx, rect, id, GUI_COLOR_BASE, opt | GUI_OPT_BORDER);

    // Draw text
    ctx->layer = base_layer+2;
    {
        gui_draw_text(ctx, buf, rect, GUI_COLOR_TEXT, 0);
    }
    
    // Draw cursor
    ctx->layer = base_layer+3;
    if (ctx->focus == id) {
        float text_width = ctx->get_text_width(ctx->style.font, buf, ctx->style.text_height);
        Gui_Rect cursor = {rect.x+(rect.w+text_width)/2, rect.y+(rect.h-ctx->style.text_height)/2, 2, ctx->style.text_height};
        gui_draw_rect(ctx, cursor, id, GUI_COLOR_TEXT, 0);
    }
    ctx->layer = base_layer;
    
    return res;
}

u32 gui_number_input(Gui_Context *ctx, char *label, f32 *value, char const *fmt, f32 min, f32 max, f32 step, u32 opt)
{
    u64 id = gui_id(label, strlen(label));
    Gui_Rect rect = gui_layout_peek_rect(ctx);
    b32 was_focus = ctx->focus == id;
    gui_update_focus(ctx, rect, id, opt | GUI_OPT_HOLD_FOCUS);

    char *v_buf;
    if (ctx->focus == id && !was_focus)
        snprintf(ctx->num_input_buf, 64, fmt, *value);
    
    if (ctx->focus == id)
    {
        v_buf = ctx->num_input_buf;
    }
    else
    {
        // Just show value if not active
        char temp_buf[64];
        snprintf(temp_buf, 64, fmt, *value);
        v_buf = temp_buf;
    }
    
    u32 res = gui_text_input(ctx, label, v_buf, 64, opt);

    if (res & GUI_RES_SUBMIT)
        parse_f32(&v_buf, value);

    return res;
}
