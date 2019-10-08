#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "lib.h"
#include "glmath2.h"
#include "shaders.h"

#include <GL/glew.h>
#include <GL/gl.h>

#define RENDERER_MAX_DEPTH 32

typedef struct Renderer_3D
{
    Shader shader;
    
    Array(Vec3f) vertices;
    Array(Vec2f) uvs;
    Array(Vec3f) normals;
    Array(Vec3f) tangents;
    Array(Vec3f) bitangents;
    
    Array(u16) indices;
    GLuint vbuff, uvbuff, nbuff, tbuff, bbuff;
} Renderer_3D;

typedef struct Renderer_2D
{
    Shader shader;

    struct {
        Array(Vec2f) vertices;
        Array(Vec2f) uvs;
    } layers[RENDERER_MAX_DEPTH];

    GLuint vbuff, uvbuff;
} Renderer_2D;

typedef struct Renderer_Text
{
    Shader shader;

    struct {
        Array(Vec2f) vertices;
        Array(vec3f) uvs;
    } layer[RENDERER_MAX_DEPTH];

    GLuint vbuff, uvbuff;
}

Renderer_3D make_renderer3d(Shader shader);
Renderer_2D make_renderer2d(Shader shader);
Renderer_Text make_renderer_text(Shader shader);


void renderer3d_begin(Renderer_3D *r);
void renderer2d_begin(Renderer_2D *r);
void renderer_text_begin(Renderer_Text *r);

void renderer3d_draw(Renderer_3D *r);
void renderer2d_draw(Renderer_2D *r, i32 layer);
void renderer_text_draw(Renderer_Text *r, i32 layer);

#endif
