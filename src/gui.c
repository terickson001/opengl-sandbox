#include "gui.h"


void draw_gui_bounded(Shader shader, Gui_Element *element, Vec2f min, Vec2f max)
{
    Vec2f pos = element->pos;
    if (element->anchor.vertical & GUI_CENTER)
        pos.y += (max.y-min.y)/2 - element->dims.y/2;
    else if (element->anchor.vertical & GUI_TOP)
        pos.y = max.y - pos.y - element->dims.y;
    else
        pos.y = min.y + pos.y;

    if (element->anchor.horizontal & GUI_CENTER)
        pos.x += (max.x-min.x)/2 - element->dims.x/2;
    else if (element->anchor.horizontal & GUI_RIGHT)
        pos.x = max.x - pos.x - element->dims.x;
    else
        pos.x = min.x + pos.x;
    
    switch (element->kind)
    {
    case GUI_BOX: {
        draw_sprite(shader,
                    element->box.sprite,
                    pos,
                    vec2f_div(element->dims, element->box.sprite->dim)
        );
    } break;
    default: break;
    }
}

void draw_gui(Shader shader, Window win, Gui_Element *element)
{
    draw_gui_bounded(shader, element, init_vec2f(0, 0), init_vec2f(win.width, win.height));
}

Gui_Element gui_box(Sprite *sprite, Vec2f pos, Vec2f dims)
{
    Gui_Box box = {sprite};

    Gui_Element elem = {0};
    elem.kind = GUI_BOX;
    elem.pos  = pos;
    elem.dims = dims;
    elem.box = box;

    return elem;
}

void gui_register_behavior(Gui *g, Gui_Element *elem)
{
    array_append(&g->active_elements, elem);
}

void update_gui(Gui *g)
{
    Vec2f cursor_pos = {0};
    double cx, cy;
    glfwGetCursorPos(g->win.handle, &cx, &cy);
    cursor_pos.x = cx;
    cursor_pos.y = g->win.height - cy;
    
    for (int i = 0; i < array_size(g->active_elements); i++)
    {
        Gui_Element *elem = g->active_elements[i];
        switch (elem->behavior->kind)
        {
        case GUI_BUTTON: {
            if (elem->pos.x <= cursor_pos.x && cursor_pos.x <= elem->pos.x + elem->dims.x &&
                elem->pos.y <= cursor_pos.y && cursor_pos.y <= elem->pos.y + elem->dims.x)
            {
                elem->behavior->button.action(elem->behavior, elem, cursor_pos);
            }
        } break;
        default: break;
        }
    }
}
