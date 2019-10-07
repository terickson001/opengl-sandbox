#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "lib.h"
#include "glmath2.h"
#include "shaders.h"

#include <GL/glew.h>
#include <GL/gl.h>

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

    Array(Vec2f) vertices;
    Array(Vec2f) uvs;

    GLuint vbuff, uvbuff;
} Renderer_2D;

Renderer_3D make_renderer3d(Shader shader);
Renderer_2D make_renderer2d(Shader shader);

void renderer3d_begin(Renderer_3D *r);
void renderer2d_begin(Renderer_2D *r);

void renderer3d_draw(Renderer_3D *r);
void renderer2d_draw(Renderer_2D *r);

#endif
