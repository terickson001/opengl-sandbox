#ifndef _ENTITY_H
#define _ENTITY_H

#include "glmath2.h"
#include "model.h"
#include "texture.h"
#include "shaders.h"
#include "sprite.h"

typedef struct Entity
{
    Model *model;
    Texture *tex;
    Vec3f pos;
    Vec3f dir;
    float h_angle, v_angle;
} Entity, Entity_3D;

typedef struct Entity_2D
{
    Sprite *sprite;

    Vec2f pos;
    Vec2f scale;
    float angle;
} Entity_2D;

Entity make_entity(Model *m, Texture *t, Vec3f pos, Vec3f dir);
void draw_entity(Shader s, Entity e);
Mat4f entity_transform(Entity e);

Entity_2D make_entity_2d(Sprite *s, Vec2f pos, Vec2f scale);
void draw_entity_2d(Shader s, Entity_2D e);

#endif // _ENTITY_H
