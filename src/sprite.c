#include "sprite.h"
#include <GL/gl.h>

#include "config.h"

HASHMAP_DEF(Animation, animation, Animation);

Sprite load_sprite(const char *filepath)
{
    FileContents fc = get_file_contents(filepath);
    char *file = fc.data;
    
    Sprite s = {0};
    animation_map_init(&s.animations);
    glGenBuffers(1, &s.vbuff);
    glGenBuffers(1, &s.uvbuff);
    
    char *map_file;
    if (!parse_file_path(&file, &map_file))
    {
        fprintf(stderr, "Failed to load sprite '%s'\n", filepath);
        exit(1);
    }
    s.map = load_texture(map_file, 0, 0);

    char *anim_name;
    Animation anim;
    
    skip_space(&file, true);
    if (!parse_f32(&file, &s.dim.x))
    {
        fprintf (stderr, "%s: Sprite must declare dimensions after texture name\n", filepath);
        exit (1);
    }
    skip_space(&file, false);
    if (!parse_f32(&file, &s.dim.y))
    {
        fprintf (stderr, "%s: Sprite must declare dimensions after texture name\n", filepath);
        exit (1);
    }
    skip_space(&file, true);

    while (parse_ident(&file, &anim_name))
    {
        anim = (Animation){0};
        skip_space(&file, true);
        if (!parse_custom_bool(&file, &anim.repeat, "repeat", "norepeat"))
        {
            fprintf(stderr, "%s: Animation '%s' must declare \'repeat\' or \'norepeat\'\n", filepath, anim_name);
            exit(1);
        }

        skip_space(&file, true);
        if (!parse_int(&file, (i64*)&anim.length))
        {
            fprintf(stderr, "%s: Animation '%s' must declare an animation length\n", filepath, anim_name);
            exit(1);
        }

        skip_space(&file, true);
        int i = 0;
        while (parse_int(&file, (i64*)&anim.keys[i].active_frame))
        {
            skip_space(&file, false);
            if (!parse_f32(&file, &anim.keys[i].uv.x))
            {
                fprintf(stderr, "%s: '%s'(%d): Invalid UV\n", filepath, anim_name, i);
                exit(1);
            }
            skip_space(&file, false);
            if (!parse_f32(&file, &anim.keys[i].uv.y))
            {
                fprintf(stderr, "%s: '%s'(%d): Invalid UV\n", filepath, anim_name, i);
                exit(1);
            }
                        skip_space(&file, false);
            if (!parse_f32(&file, &anim.keys[i].dim.x))
            {
                fprintf(stderr, "%s: '%s'(%d): Invalid dimensions\n", filepath, anim_name, i);
                exit(1);
            }
            skip_space(&file, false);
            if (!parse_f32(&file, &anim.keys[i].dim.y))
            {
                fprintf(stderr, "%s: '%s'(%d): Invalid dimensions\n", filepath, anim_name, i);
                exit(1);
            }
            skip_space(&file, true);
            i++;
        }
        printf("%s: ADDING ANIMATION '%s'\n", filepath, anim_name);
        printf("REPEAT?: %s\n", anim.repeat?"True":"False");
        printf("LENGTH: %d\n", anim.length);
        for (int i = 0; i < 4; i++)
            printf("%d (%f, %f) %f x %f\n", anim.keys[i].active_frame, anim.keys[i].uv.x, anim.keys[i].uv.y, anim.keys[i].dim.x, anim.keys[i].dim.y);
        sprite_add_anim(&s, anim_name, anim);
    }

    return s;
}

Sprite make_sprite(Texture map)
{
    Sprite s = {0};
    s.map = map;
    animation_map_init(&s.animations);
    glGenBuffers(1, &s.vbuff);
    glGenBuffers(1, &s.uvbuff);
    
    return s;
}

void sprite_add_anim(Sprite *s, char *name, Animation anim)
{
    animation_map_set(&s->animations, hash_crc64(name, strlen(name)), anim);
}

void sprite_set_anim(Sprite *s, char *name)
{
    Animation *anim = animation_map_get(&s->animations, hash_crc64(name, strlen(name)));
    if (!anim->repeat)
    {
        if (!s->prev_anim)
            s->prev_anim = s->curr_anim;
    }
    else
    {
        s->prev_anim = 0;
    }
    s->anim_frame = 0;
    s->curr_anim = anim;
    s->key_index = 0;
}

void draw_sprite(Shader shader, Sprite *s, Vec2f pos, Vec2f scale)
{
    if (s->anim_frame == s->curr_anim->length)
    {
        s->anim_frame = 0;
        s->key_index = 0;
        if (!s->curr_anim->repeat)
        {
            s->curr_anim = s->prev_anim;
            s->prev_anim = 0;
        }
    }

    if (s->anim_frame > s->curr_anim->keys[s->key_index].active_frame &&
        s->anim_frame == s->curr_anim->keys[s->key_index+1].active_frame)
        s->key_index++;

    Animation_Key key = s->curr_anim->keys[s->key_index];
    Vec2f vertices[6], uvs[6];

    Vec2f scaled_dim = vec2f_mul(key.dim, scale);
    
    vertices[0] = init_vec2f(pos.x,              pos.y);
    vertices[1] = init_vec2f(pos.x+scaled_dim.x, pos.y+scaled_dim.y);
    vertices[2] = init_vec2f(pos.x,              pos.y+scaled_dim.y);
    
    vertices[3] = vertices[0];
    vertices[4] = init_vec2f(pos.x+scaled_dim.x, pos.y);
    vertices[5] = vertices[1];

    Vec2f unit_uv = vec2f_mul(key.uv, init_vec2f(1.0f/(float)s->map.width, 1.0f/(float)s->map.height));
    Vec2f unit_dim = vec2f_mul(key.dim, init_vec2f(1.0f/(float)s->map.width, 1.0f/(float)s->map.height));

    uvs[0] = unit_uv;
    uvs[1] = vec2f_add(unit_uv, unit_dim);
    uvs[2] = init_vec2f(unit_uv.x, unit_uv.y+unit_dim.y);

    uvs[3] = uvs[0];
    uvs[4] = init_vec2f(unit_uv.x+unit_dim.x, unit_uv.y);
    uvs[5] = uvs[1];

    glBindBuffer(GL_ARRAY_BUFFER, s->vbuff);
    glBufferData(GL_ARRAY_BUFFER, 6*sizeof(Vec2f), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, s->uvbuff);
    glBufferData(GL_ARRAY_BUFFER, 6*sizeof(Vec2f), uvs, GL_STATIC_DRAW);

    glUseProgram(shader.id);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, s->map.diffuse);
    
    glUniform2i(shader.uniforms.resolution, 1024, 768);
    glUniform1i(shader.uniforms.diffuse_tex, 0);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, s->vbuff);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, s->uvbuff);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisable(GL_BLEND);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    /* printf("ANIM_FRAME: %d; KEY_INDEX: %d\n", s->anim_frame, s->key_index); */
    /* printf("CURR_ANIM: %p; PREV_ANIM: %p\n", s->curr_anim, s->prev_anim); */
    s->anim_frame++;
}
