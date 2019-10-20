#include "gui.h"
#include "config.h"

// @Robustness(Tyler): Actually support the `opt` parameter correctly

u64 gui_id(const void *data, isize size)
{
    return hash_crc64(data, size);
}

void gui_input_mouse(Gui_Context *ctx, KeyState *buttons, Vec2f pos, Vec2f scroll)
{
    memcpy(ctx->mouse, buttons, sizeof(buttons[0])*3);
    ctx->cursor = pos;
    ctx->scroll = scroll;
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

float gui_text_widthn(Gui_Context *ctx, char const *text, int n, int size)
{
    if (!text)
        return 0;

    int len = n;
    if (len == -1)
        len = strlen(text);

    if (len == 0)
        return 0;

    float width = 0;
    for (int i = 0; i < len; i++)
        width += ctx->get_char_width(ctx->style.font, text[i], size);
    return width;
}

float gui_text_width(Gui_Context *ctx, char const *text, int size)
{
    return gui_text_widthn(ctx, text, -1, size);
}

void gui_push_container(Gui_Context *ctx, Gui_Rect container)
{
    array_append(&ctx->containers, container);
}

Gui_Rect gui_curr_container(Gui_Context *ctx)
{
    return ctx->containers[array_size(ctx->containers)-1];
}

void gui_pop_container(Gui_Context *ctx)
{
    array_set_size(&ctx->containers, array_size(ctx->containers)-1);
}

Gui_Context gui_init()
{
    Gui_Context ctx = {0};
    ctx.style = GUI_DEFAULT_STYLE;
    array_init(&ctx.draws);
    array_init(&ctx.windows);
    array_init(&ctx.containers);
    
    return ctx;
}

void gui_begin(Gui_Context *ctx, Window win)
{
    array_set_size(&ctx->draws, 0);
    array_set_size(&ctx->windows, 0);
    array_set_size(&ctx->containers, 0);

    ctx->draw_index = 0;

    gui_push_container(ctx, (Gui_Rect){0, 0, win.width, win.height});
    
    ctx->layout.pos = init_vec2f(0, 0);
    ctx->layout.items = 1;
    memset(ctx->layout.widths, 0, sizeof(ctx->layout.widths[0])*MAX_ROW_ITEMS);
    ctx->layout.curr_item = 0;

    ctx->layer = 0;
    ctx->cursor_icon = GUI_CURSOR_ARROW;
}

int _win_zcmp(const void *a, const void *b)
{
    return (*(Gui_Window**)a)->layer - (*(Gui_Window**)b)->layer;
}

void gui_end(Gui_Context *ctx)
{
    // Reset input state
    memset(ctx->text_input, 0, 128);
    ctx->scroll = (Vec2f){0};
    ctx->last_cursor = ctx->cursor;
    ctx->cursor = (Vec2f){0};
    
    // Sort windows by layer
    qsort(ctx->windows, array_size(ctx->windows), sizeof(Gui_Window *),  _win_zcmp);
    
    // Stack windows
    i32 prev_max = 0;
    i32 new_max  = 0;
    for (int i = 0; i < array_size(ctx->windows); i++)
    {
        Gui_Window *win = ctx->windows[i];
        win->layer = i; // Re-Pack layers
        
        for (int d = win->draws.start; d <= win->draws.end; d++)
        {
            ctx->draws[d].layer += prev_max + 1;
            new_max = MAX(new_max, ctx->draws[d].layer);
        }

        prev_max = new_max;
    }

    ctx->win_top_layer = array_size(ctx->windows);
    ctx->layer = 0;
}

void gui_row(Gui_Context *ctx, i32 items, i32 *widths, i32 height)
{
    ctx->layout.size.y = height ? height : ctx->style.size.y;
    ctx->layout.items = items;
    memcpy(ctx->layout.widths, widths, items*sizeof(widths[0]));
    ctx->layout.curr_item = 0;
}

Gui_Rect gui_layout_peek_rect(Gui_Context *ctx)
{
    Gui_Rect rect = {0};
    Gui_Rect container = gui_curr_container(ctx);

    rect.x = container.x + ctx->layout.pos.x;
    rect.y = container.y + ctx->layout.pos.y;

    rect.w = ctx->layout.widths[ctx->layout.curr_item];
    rect.h = ctx->layout.size.y;

    // Position relative to right for negative values
    if (rect.w <= 0) rect.w += container.x + container.w - rect.x;

    // Adjust for spacing
    rect.x += ctx->style.spacing;
    rect.y += ctx->style.spacing;
    rect.w -= ctx->style.spacing * 2;
    rect.h -= ctx->style.spacing * 2;

    return rect;
}

Gui_Rect gui_layout_rect(Gui_Context *ctx)
{
    Gui_Rect rect = gui_layout_peek_rect(ctx);
    // Gui_Rect container = gui_curr_container(ctx);

    // Advance layout position
    ctx->layout.pos.x += rect.w + ctx->style.spacing * 2;

    ctx->layout.curr_item++;
    if (ctx->layout.curr_item == ctx->layout.items)
    {
        // ctx->layout.pos.x -= container.w;
        ctx->layout.pos.x = 0;
        ctx->layout.pos.y += ctx->layout.size.y;
        ctx->layout.curr_item = 0;
    }

    return rect;
}

Gui_Rect gui_text_rect(Gui_Context *ctx, char *str)
{
    Gui_Rect rect = {0};

    rect.h = ctx->style.text_height;
    rect.w = gui_text_width(ctx, str, rect.h);

    return rect;
}

Gui_Rect gui_align_rect(Gui_Context *ctx, Gui_Rect bound, Gui_Rect rect, u32 opt)
{
    Gui_Rect ret = {0};
    ret.w = rect.w;
    ret.h = rect.h;

    bound.x += ctx->style.padding;
    bound.y += ctx->style.padding;
    bound.w -= ctx->style.padding*2;
    bound.h -= ctx->style.padding*2;

    if (opt & GUI_OPT_RIGHT)
        ret.x = bound.x+bound.w - rect.w;
    else if (opt & GUI_OPT_LEFT)
        ret.x = bound.x;
    else
        ret.x = bound.x + (bound.w - rect.w)/2;

    if (opt & GUI_OPT_BOTTOM)
        ret.y = bound.y+bound.h - rect.h;
    else if (opt & GUI_OPT_TOP)
        ret.y = bound.y;
    else
        ret.y = bound.y + (bound.h - rect.h)/2;

    return ret;
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

    pos.x = rect.x;
    pos.y = rect.y;

    Gui_Draw *draw = gui_add_draw(ctx, GUI_DRAW_TEXT);
    draw->layer = ctx->layer;
    draw->text.pos = pos;
    draw->text.size = rect.h;
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

    Gui_Draw *draw = gui_add_draw(ctx, GUI_DRAW_RECT);
    draw->layer = ctx->layer;
    draw->rect.rect = rect;
    draw->rect.color = color;
    draw->rect.color_id = color_id;

    if (opt & GUI_OPT_BORDER)
        gui_draw_border(ctx, rect, id);
}

b32 gui_next_draw(Gui_Context *ctx, Gui_Draw *ret)
{
    if (ctx->draw_index >= array_size(ctx->draws))
        return false;

    *ret = ctx->draws[ctx->draw_index++];
    return true;
}

void gui_label(Gui_Context *ctx, char *str, u32 opt)
{
    Gui_Rect bounds = gui_layout_rect(ctx);
    Gui_Rect text_rect = gui_text_rect(ctx, str);
    gui_draw_text(ctx, str, gui_align_rect(ctx, bounds, text_rect, opt), GUI_COLOR_TEXT, opt);
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

    ctx->layer = base_layer+1;
    {
        Gui_Rect text_rect = gui_text_rect(ctx, label);
        Gui_Rect text_aligned = gui_align_rect(ctx, rect, text_rect, opt);
        gui_draw_text(ctx, label, text_aligned, GUI_COLOR_TEXT, opt);
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
    else if (ctx->hover == id && ctx->scroll.y)
    {
        float add = ctx->scroll.y * (step ? step : 1.0);
        *value = CLAMP(*value + add, min, max);
    }

    *value = *value < min
        ? min : *value > max
        ? max : *value;

    u32 res = 0;
    if (*value != prev) res |= GUI_RES_UPDATE; // Value changed

    i32 base_layer = ctx->layer;
    // Draw slider
    gui_draw_rect(ctx, rect, id, GUI_COLOR_BASE, opt ^ GUI_OPT_BORDER);

    // Draw thumb
    ctx->layer = base_layer+1;
    {
        f32 percentage = (*value - min)/(max-min);
        Gui_Rect thumb = {rect.x + percentage * (rect.w-tw), rect.y, tw, rect.h};
        gui_draw_rect(ctx, thumb, id, GUI_COLOR_BUTTON, opt);
    }
    // Draw value
    ctx->layer = base_layer+2;
    {
        char val_buf[128];
        snprintf(val_buf, 128, fmt, *value);
        Gui_Rect text_rect = gui_text_rect(ctx, val_buf);
        Gui_Rect text_aligned = gui_align_rect(ctx, rect, text_rect, opt);
        gui_draw_text(ctx, val_buf, text_aligned, GUI_COLOR_TEXT, opt);
    }
    ctx->layer = base_layer;
    return res;
}

int _insert_string_at(char *dst, char *src, int idx, int max)
{
    int dlen = strlen(dst);
    int slen = strlen(src);
    slen = MIN(slen, max - dlen);

    char *temp = malloc(max);
    memcpy(temp, dst, idx);
    memcpy(temp+idx, src, slen);
    memcpy(temp+idx+slen, dst+idx, dlen-idx);

    memcpy(dst, temp, dlen+slen);

    free(temp);

    return slen;
}

int _remove_string_between(char *str, int idx_start, int idx_end)
{
    if (idx_start == idx_end)
        return 0;

    int len = strlen(str);
    int ret = idx_end - idx_start;
    if (idx_start > idx_end)
    {
        int tmp = idx_start;
        idx_start = idx_end;
        idx_end = tmp;
        ret = 0;
    }

    memcpy(str+idx_start, str+idx_end, len-idx_end);
    memset(str+idx_start+(len-idx_end), 0, idx_end);

    return ret;
}
void _remove_char_at(char *str, int idx)
{
    int len = strlen(str);

    memcpy(str+idx-1, str+idx, len-idx+1);
}

void _update_cursor(Gui_Context *ctx, int change, int max)
{
    if (key_down(GLFW_KEY_LEFT_SHIFT) && ctx->text_box.mark == -1)
        ctx->text_box.mark = ctx->text_box.cursor;
    else if (!key_down(GLFW_KEY_LEFT_SHIFT) && ctx->text_box.mark != -1)
    {
        i32 mark = ctx->text_box.mark;
        ctx->text_box.mark = -1;
        
        if (change == -1)
        {
            ctx->text_box.cursor = MIN(ctx->text_box.cursor, mark);
            return;
        }
        else if (change == 1)
        {
            ctx->text_box.cursor = MAX(ctx->text_box.cursor, mark);
            return;
        }
    }

    ctx->text_box.cursor += change;
    ctx->text_box.cursor = CLAMP(ctx->text_box.cursor, 0, max);

    if (ctx->text_box.cursor == ctx->text_box.mark)
        ctx->text_box.mark = -1;
}

i32 _word_beg(Gui_Context *ctx, char *buf, i32 len)
{
    i32 index = ctx->text_box.cursor;
    while (index-1 && !char_is_alphanum(buf[index-1]))
        index--;
    while (index-1 && char_is_alphanum(buf[index-1]))
        index--;
    if (index == 1 && char_is_alphanum(buf[index]))
        index--;
    return MAX(index, 0);
}

i32 _word_end(Gui_Context *ctx, char *buf, i32 len)
{
    i32 index = ctx->text_box.cursor;
    while (index < len && !char_is_alphanum(buf[index]))
        index++;
    while (index < len && char_is_alphanum(buf[index]))
        index++;
    return MIN(index, len);
}

u32 gui_text_input(Gui_Context *ctx, char *label, char *buf, int buf_size, u32 opt)
{
    u64 id = gui_id(label, strlen(label));

    Gui_Rect rect = gui_layout_rect(ctx);
    b32 was_focus = ctx->focus == id;
    gui_update_focus(ctx, rect, id, opt ^ GUI_OPT_HOLD_FOCUS);

    if (ctx->hover == id)
        ctx->cursor_icon = GUI_CURSOR_BAR;
    
    u32 res = 0;
    i32 cursor_start = ctx->text_box.cursor;
    Gui_Rect text_rect;
    int len = strlen(buf);
    if (ctx->focus == id)
    {
        if (!was_focus)
        {
            ctx->text_box.mark = len > 0 ? 0 : -1;
            ctx->text_box.cursor = len;
        }

        if (*ctx->text_input)
        {
            if (ctx->text_box.mark != -1)
            {
                ctx->text_box.cursor -= _remove_string_between(buf, ctx->text_box.mark, ctx->text_box.cursor);
                ctx->text_box.mark = -1;
            }
            ctx->text_box.cursor += _insert_string_at(buf, ctx->text_input, ctx->text_box.cursor, buf_size-1);
        }

        // Backspace
        if (key_repeat(GLFW_KEY_BACKSPACE))
        {
            if (ctx->text_box.mark != -1) // Delete Marked Region
            {
                ctx->text_box.cursor -= _remove_string_between(buf, ctx->text_box.mark, ctx->text_box.cursor);
            }
            else if (key_down(GLFW_KEY_LEFT_CONTROL))
            {
                i32 word_index = _word_beg(ctx, buf, len);
                ctx->text_box.cursor -= _remove_string_between(buf, word_index, ctx->text_box.cursor);
            }
            else if (ctx->text_box.cursor > 0)
            {
                _remove_char_at(buf, ctx->text_box.cursor--);
            }
            ctx->text_box.mark = -1;
        }
        else if (key_repeat(GLFW_KEY_DELETE))
        {
            if (ctx->text_box.mark != -1) // Delete Marked Region
                ctx->text_box.cursor -= _remove_string_between(buf, ctx->text_box.mark, ctx->text_box.cursor);
            else if (key_down(GLFW_KEY_LEFT_CONTROL))
            {
                i32 word_index = _word_end(ctx, buf, len);
                _remove_string_between(buf, ctx->text_box.cursor, word_index);
            }
            else if (ctx->text_box.cursor < len)
                _remove_char_at(buf, ctx->text_box.cursor + 1);
            ctx->text_box.mark = -1;
        }

        // Enter
        if (key_pressed(GLFW_KEY_ENTER))
        {
            ctx->focus = 0;
            res |= GUI_RES_SUBMIT;
        }

        // @Note(Tyler): Should I change these to emacs-like keybinds?

        // Cursor Movement
        if (key_down(GLFW_KEY_LEFT_CONTROL) && key_pressed('A')) // Mark all
        {
            ctx->text_box.mark = 0;
            ctx->text_box.cursor = len;
        }

        if (key_repeat(GLFW_KEY_LEFT))
        {
            int change = -1;
            if (key_down(GLFW_KEY_LEFT_CONTROL)) // Move word-wise
                change = _word_beg(ctx, buf, len) - ctx->text_box.cursor;
            _update_cursor(ctx, change, len);
        }
        else if (key_repeat(GLFW_KEY_RIGHT))
        {
            int change = +1;
            if (key_down(GLFW_KEY_LEFT_CONTROL)) // Move word-wise
                change = _word_end(ctx, buf, len) - ctx->text_box.cursor;
            _update_cursor(ctx, change, len);
        }
        else if (key_pressed(GLFW_KEY_HOME) || key_pressed(GLFW_KEY_UP))
            _update_cursor(ctx, -len, len);
        else if (key_pressed(GLFW_KEY_END) || key_pressed(GLFW_KEY_DOWN))
            _update_cursor(ctx, +len, len);
    }

    text_rect = gui_text_rect(ctx, buf);
    text_rect = gui_align_rect(ctx, rect, text_rect, opt);

    // Text-selection with mouse 
    if (ctx->focus == id && gui_mouse_down(ctx, 0) && len > 0)
    {
        float pos = text_rect.x;
        float cw = 0;
        int index = 0;
        while (index < len+1 && pos < ctx->cursor.x)
        {
            cw = ctx->get_char_width(ctx->style.font, buf[index++], ctx->style.text_height);
            pos += cw;
        }
        index = MAX(index-1, 0);
        if (cw && pos - ctx->cursor.x < (ctx->cursor.x - (pos-cw)) && index < len)
            index++;


        if (gui_mouse_pressed(ctx, 0))
        {
            ctx->text_box.cursor = index;
            ctx->text_box.mark = -1;
        }
        else
        {
            if (ctx->text_box.mark == -1 && ctx->text_box.cursor != index)
                ctx->text_box.mark = ctx->text_box.cursor;
            ctx->text_box.cursor = index;
        }
    }

    // Scrolling Text
    float cursor_pos = 0;
    float offset_x = 0;
    if (ctx->focus == id)
    {
        offset_x    = gui_text_widthn(ctx, buf, ctx->text_box.offset, ctx->style.text_height);
        text_rect.x -= offset_x;

        cursor_pos = gui_text_widthn(ctx, buf, ctx->text_box.cursor, ctx->style.text_height);
        
        float box_min = text_rect.x + offset_x;
        float box_max = box_min + (rect.w - ctx->style.padding*2);
        float cursor_rel = text_rect.x + cursor_pos;
        if (cursor_rel > box_max) // Scroll forwards
        {
            float diff = cursor_rel - box_max;
            float sum = 0;
            int i = ctx->text_box.cursor - 1;
            while (sum < diff)
            {
                sum += ctx->get_char_width(ctx->style.font, buf[i--], ctx->style.text_height);
                ctx->text_box.offset++;
            }
            text_rect.x -= sum;
        }   
        else if (cursor_rel < box_min) // Scroll backwards
        {
            float diff = box_min - cursor_rel;
            float sum = 0;
            int i = ctx->text_box.cursor;
            while (sum < diff)
            {
                sum += ctx->get_char_width(ctx->style.font, buf[i++], ctx->style.text_height);
                ctx->text_box.offset--;
            }
            text_rect.x += sum;
        }
        else if (ctx->text_box.offset > 0) // Scroll backwards if text can fit
        {
            // @Note(Tyler): Get width of one extra character to make sure the whole character will fit
            float offset_to_end = gui_text_width(ctx, buf+ctx->text_box.offset-1, ctx->style.text_height);
            if (box_min + offset_to_end < box_max)
            {
                float diff = box_max - (box_min + offset_to_end);
                float sum = 0;
                int i = ctx->text_box.offset-1;
                while (sum < diff)
                {
                    sum += ctx->get_char_width(ctx->style.font, buf[i--], ctx->style.text_height);
                    ctx->text_box.offset--;
                }
                text_rect.x += sum;
            }
        }
    }

    if (cursor_start != ctx->text_box.cursor)
    {
        ctx->text_box.cursor_last_updated = ctx->time;
    }

    i32 base_layer = ctx->layer;
    // Draw box
    gui_draw_rect(ctx, rect, id, GUI_COLOR_BASE, opt ^ GUI_OPT_BORDER);

    // Draw text
    ctx->layer = base_layer+1;
    {
        gui_draw_text(ctx, buf, text_rect, GUI_COLOR_TEXT, opt);
    }

    if (ctx->focus == id) {
        float text_width = text_rect.w;
        
        // Draw Mark
        if (ctx->text_box.mark != -1)
        {
            // @Todo(Tyler): Fix visual artefacts due to mark width
            ctx->layer++;
            float mark_pos   = gui_text_widthn(ctx, buf, ctx->text_box.mark, ctx->style.text_height);
            float mark_x     = MIN(mark_pos, cursor_pos); // Resolve cursor behind mark
            float mark_width = ABS(mark_pos - cursor_pos);
            float mark_diff  = text_width - mark_x;
            Gui_Rect mark    = {text_rect.x+text_rect.w-mark_diff, text_rect.y, mark_width, text_rect.h};
            gui_draw_rect(ctx, mark, id, GUI_COLOR_MARK, 0);
        }

        // Draw cursor
        if (sin(ctx->time*5) > 0 || // Blink
            ctx->time - ctx->text_box.cursor_last_updated < 0.4) // Display if recently updated
        {
            ctx->layer++;
            float diff = text_width - cursor_pos;

            Gui_Rect cursor = {text_rect.x+text_rect.w-diff, text_rect.y, 2, text_rect.h};
            gui_draw_rect(ctx, cursor, id, GUI_COLOR_TEXT, 0);
        }
    }
    ctx->layer = base_layer;

    return res;
}

u32 gui_number_input(Gui_Context *ctx, char *label, f32 *value, char const *fmt, f32 min, f32 max, f32 step, u32 opt)
{
    u64 id = gui_id(label, strlen(label));
    Gui_Rect rect = gui_layout_peek_rect(ctx);
    b32 was_focus = ctx->focus == id;
    gui_update_focus(ctx, rect, id, opt ^ GUI_OPT_HOLD_FOCUS);

    char *v_buf;
    if (ctx->focus == id)
    {
        if (!was_focus)
        {
            ctx->text_box.mark = 0;
            ctx->text_box.cursor = snprintf(ctx->num_input_buf, 64, fmt, *value);
        }
        v_buf = ctx->num_input_buf;
    }
    else
    {
        if (ctx->hover == id && ctx->scroll.y)
        {
            float add = ctx->scroll.y * (step ? step : 1.0);
            *value = CLAMP(*value + add, min, max);
        }
        // Show value if not active
        char temp_buf[64];
        snprintf(temp_buf, 64, fmt, *value);
        v_buf = temp_buf;
    }

    u32 res = gui_text_input(ctx, label, v_buf, 64, opt);

    if (res & GUI_RES_SUBMIT)
        parse_f32(&v_buf, value);

    return res;
}

Gui_Rect gui_window_container(Gui_Context *ctx, Gui_Window *win)
{
    win->container = win->rect;
    win->container.y += ctx->style.title_size + ctx->style.padding;
    win->container.h -= ctx->style.title_size*2 + ctx->style.padding*2;
    win->container.x += ctx->style.padding;
    win->container.w -= ctx->style.padding * 2;
    return win->container;
}

Gui_Window gui_window_init(Gui_Context *ctx, char *title, Gui_Rect rect)
{
    Gui_Window window = {0};
    window.title = title;
    window.rect = rect;
    window.open = true;
    
    return window;
}

void gui_bring_to_front(Gui_Context *ctx, Gui_Window *win)
{
    win->layer = ++ctx->win_top_layer;
}

void gui_update_window_focus(Gui_Context *ctx, Gui_Window *win, u64 id, u32 opt)
{
    b32 mouse_over = gui_mouse_over(ctx, win->rect);
    if (mouse_over && !gui_mouse_down(ctx, 0) &&
        (!ctx->window_hover || ctx->window_hover->layer < win->layer)) ctx->window_hover = win;
    if (ctx->window_focus == win)
    {
        if (gui_mouse_pressed(ctx, 0) && !mouse_over) ctx->window_focus = 0;
    }
    if (ctx->window_hover == win)
    {
        if (!mouse_over) ctx->window_hover = 0;
        else if (gui_mouse_pressed(ctx, 0)) ctx->window_focus = win;
    }
}

u32 gui_window(Gui_Context *ctx, Gui_Window *win, u32 opt)
{
    u64 id = gui_id(win->title, strlen(win->title));
    
    if (!win->open) return 0;
    
    gui_push_container(ctx, gui_window_container(ctx, win));
    win->draws.start = array_size(ctx->draws);
    array_append(&ctx->windows, win);

    Gui_Rect title = win->rect;
    title.h = ctx->style.title_size;

    Gui_Rect close = title;
    close.w = close.h;
    close.x += title.w-close.w;

    // Handle hover/focus
    b32 was_focus = ctx->window_focus == win;
    b32 close_hover = false;
    gui_update_window_focus(ctx, win, id, opt);
    if (ctx->window_hover == win || ctx->window_focus == win)
    {
        if (ctx->window_focus == win && !was_focus) gui_bring_to_front(ctx, win);

        // Temp labels for window elements
        char temp_label[256];
        i32 label_len;
        
        // Handle title drag
        label_len = snprintf(temp_label, 256, "%s.__TITLE__", win->title);
        u64 title_id = gui_id(temp_label, label_len);
        was_focus = ctx->focus == title_id;
        gui_update_focus(ctx, title, title_id, opt);
        if (ctx->focus == title_id)
        {
            win->rect.x += ctx->cursor.x - ctx->last_cursor.x;
            win->rect.y += ctx->cursor.y - ctx->last_cursor.y;
        }
        
        // Handle close button
        label_len = snprintf(temp_label, 256, "%s.__CLOSE__", win->title);
        u64 close_id = gui_id(temp_label, label_len);
        was_focus = ctx->focus == close_id;
        gui_update_focus(ctx, close, close_id, 0);
        close_hover = ctx->hover == close_id;
        if (was_focus && gui_mouse_released(ctx, 0) && ctx->hover == close_id)
        {
            ctx->window_focus = 0;
            ctx->window_hover = 0;
            win->open = false;
        }
    }

    i32 base_layer = ctx->layer;
    // Draw background
    gui_draw_rect(ctx, win->rect, id, GUI_COLOR_WINDOW, opt);

    // Draw title
    {
        ctx->layer++;
        gui_draw_rect(ctx, title, id, GUI_COLOR_TITLE, opt);
        
        ctx->layer++;
        Gui_Rect title_text = gui_text_rect(ctx, win->title);
        title_text = gui_align_rect(ctx, title, title_text, opt);
        gui_draw_text(ctx, win->title, title_text, GUI_COLOR_TITLE_TEXT, opt);

        // Draw close button
        // @Todo(Tyler): Icons
        Gui_Color close_color = GUI_COLOR_TITLE;
        if (close_hover)
            close_color = GUI_COLOR_CLOSE;
        gui_draw_rect(ctx, close, id, close_color, opt);
    }

    ctx->layer = base_layer;
    return GUI_RES_ACTIVE;
}

void gui_window_end(Gui_Context *ctx)
{
    Gui_Window *win = ctx->windows[array_size(ctx->windows)-1];
    win->draws.end = array_size(ctx->draws)-1;
    gui_pop_container(ctx);
}
