#ifndef _TEXTURE_H
#define _TEXTURE_H

#include <GL/glew.h>
#include <GL/gl.h>

#include "shaders.h"

typedef struct Texture
{
    GLuint diffuse;
    GLuint normal;
    GLuint specular;
} Texture;

Texture load_texture(const char *diff_path, const char *norm_path, const char *spec_path);
void activate_texture(Shader s, Texture t);
void disable_texture(Shader s, Texture t);
void destroy_texture(Texture t);

#endif // _TEXTURE_H
