#ifndef _TEXTURE_H
#define _TEXTURE_H

#include <GL/glew.h>
#include <GL/gl.h>

#include "shaders.h"
#include "glmath2.h"
#include "image.h"

typedef struct Texture
{
    union {
        TextureInfo info;
        struct {
            u32 width, height;
        };
    };
    GLuint diffuse;
    GLuint normal;
    GLuint specular;
} Texture;

Texture load_texture(const char *diff_path, const char *norm_path, const char *spec_path);
void activate_texture(Shader s, Texture t);
void disable_texture(Shader s, Texture t);
void destroy_texture(Texture t);
Texture color_texture(Vec4f color, b32 normalize);
Texture texture_pallete(Vec4f *colors, int n, b32 normalize);
Vec2f texture_pallete_index(Texture pallete, int i);

#endif // _TEXTURE_H
