#ifndef _ENTITY_H
#define _ENTITY_H

#include "glmath2.h"
#include "model.h"
#include "texture.h"
#include "shaders.h"

typedef struct Entity
{
    Model *model;
    Texture *tex;
    Vec3f pos;
    Vec3f dir;
    float h_angle, v_angle;
} Entity;

Entity make_entity(Model *m, Texture *t, Vec3f pos, Vec3f dir);
Mat4f entity_transform(Entity e);
void draw_entity(Shader s, Entity e);

#endif // _ENTITY_H
