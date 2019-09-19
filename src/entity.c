#include "entity.h"

#include <math.h>

#include "util.h"

Entity make_entity(Model *m, Texture *t, Vec3f pos, Vec3f dir)
{
    Entity e;
    
    e.model = m;
    e.tex = t;
    e.pos = pos;
    e.scale = init_vec3f(1, 1, 1);
    e.dir = vec3f_normalize(dir);
    get_direction_angles(dir, &e.h_angle, &e.v_angle);
    
    return e;
}

Mat4f entity_transform(Entity e)
{
    Mat4f scale = mat4f_scale(e.scale);
    Mat4f translate = mat4f_translate(init_mat4f(1), e.pos);
    Mat4f rotate;
    {
        Vec3f right = init_vec3f(
            sin(e.h_angle - M_PI/2),
            0,
            cos(e.h_angle - M_PI/2)
        );
        Vec3f up = vec3f_cross(right, e.dir);
    
        rotate = mat4f_rotate_to(e.pos, vec3f_add(e.pos, e.dir), up);
    }
    return mat4f_mul(translate, mat4f_mul(rotate, scale));
}

void draw_entity(Shader s, Entity e)
{
    Mat4f model = entity_transform(e);

    glVertexAttrib4fv(5, model.data);
    glVertexAttrib4fv(6, model.data+4);
    glVertexAttrib4fv(7, model.data+8);
    glVertexAttrib4fv(8, model.data+12);
    
    activate_texture(s, *e.tex);
    draw_model(s, *e.model);
    disable_texture(s, *e.tex);
}
