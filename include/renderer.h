#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "lib.h"
#include "glmath2.h"
#include "shaders.h"
#include "texture.h"

#include <GL/glew.h>
#include <GL/gl.h>

#define RENDERER_MAX_DEPTH 32

typedef struct Renderer_2D
{
    Shader shader;
    Shader text_shader;
    
    Texture tex;
    Texture font_tex;
    
    struct {
        Array(Vec2f) vertices;
        Array(Vec2f) uvs;

        Array(Vec2f) text_vertices;
        Array(Vec3f) text_uvs;
    } layers[RENDERER_MAX_DEPTH];

    GLuint vbuff, uvbuff;
} Renderer_2D;

Renderer_2D make_renderer2d(Shader shader, Shader text_shader, Texture tex, Texture font_tex);
void renderer2d_begin(Renderer_2D *r);
void renderer2d_draw(Renderer_2D *r);

#endif
