#ifndef _SPRITE_H_
#define _SPRITE_H_

#include "texture.h"
#include "glmath2.h"
#include "shaders.h"
#include "lib.h"

typedef struct Animation_Key
{
    u8 active_frame;
    Vec2f uv;
    Vec2f dim;
} Animation_Key;

typedef struct Animation
{
    // The length of the animation in frames
    b32 repeat;
    u8 length;
    Animation_Key keys[16];
} Animation;

HASHMAP_DEC(Animation, animation, Animation);

typedef struct Sprite
{
    Texture map;
    Vec2f dim;
    Animation_Map animations;
    
    u16 anim_frame;
    Animation *curr_anim, *prev_anim;
    u8 key_index;

    Shader shader;
    GLuint vbuff, uvbuff;
} Sprite;

Sprite load_sprite(const char *filepath);
Sprite make_sprite(Texture map);
void sprite_add_anim(Sprite *s, char *name, Animation anim);
void sprite_set_anim(Sprite *s, char *name);
void draw_sprite(Shader shader, Sprite *s, Vec2f pos, Vec2f scale);

#endif
