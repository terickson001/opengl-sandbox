#include "renderer.h"

Renderer_3D make_renderer3d(Shader shader)
{
    Renderer_3D r = {0};
    
    r.shader = shader;
    
    array_init(&r.vertices);
    array_init(&r.uvs);
    array_init(&r.normals);
    array_init(&r.tangents);
    array_init(&r.bitangents);

    array_init(&r.indices);

    glGenBuffers(5, &r.vbuff);

    return r;
}

Renderer_2D make_renderer2d(Shader shader)
{
    Renderer_2D r = {0};

    r.shader = shader;

    for (int i = 0; i < RENDERER_MAX_DEPTH; i++)
    {
        array_init(&r->layer[i].vertices);
        array_init(&r->layer[i].uvs);
    }

    glGenBuffers(2, &r.vbuff);

    return r;
}

Renderer_Text make_renderer_text(Shader shader)
{
    Renderer_Text r = {0};

    r.shader = shader;
    
    for (int i = 0; i < RENDERER_MAX_DEPTH; i++)
    {
        array_init(&r->layer[i].vertices);
        array_init(&r->layer[i].uvs);
    }

    glGenBuffers(2, &r.vbuff);

    return r;
}

void renderer3d_begin(Renderer_3D *r)
{
    array_set_size(&r->vertices, 0);
    array_set_size(&r->uvs, 0);
    array_set_size(&r->normals, 0);
    array_set_size(&r->tangents, 0);
    array_set_size(&r->bitangents, 0);

    array_set_size(&r->indices, 0);

    glUseProgram(r->shader.id);
}

void renderer2d_begin(Renderer_2D *r)
{
    for (int i = 0; i < RENDERER_MAX_DEPTH; i++)
    {
        array_set_size(&r->layer[i].vertices, 0);
        array_set_size(&r->layer[i].uvs, 0);
    }
}

void renderer_text_begin(Renderer_Text *r)
{
    for (int i = 0; i < RENDERER_MAX_DEPTH; i++)
    {
        array_set_size(&r->layer[i].vertices, 0);
        array_set_size(&r->layer[i].uvs, 0);
    }
}

void renderer3d_draw(Renderer_3D *r)
{
    // TODO
}

void renderer2d_draw(Renderer_2D *r, i32 layer)
{
    glUseProgram(r->shader.id);
    
    glBindBuffer(GL_ARRAY_BUFFER, r->vbuff);
    glBufferData(GL_ARRAY_BUFFER, array_size(r->layer[layer].vertices)*sizeof(Vec2f), r->layer[layer].vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, r->uvbuff);
    glBufferData(GL_ARRAY_BUFFER, array_size(r->layer[layer].uvs)*sizeof(Vec2f), r->layer[layer].uvs, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, r->vbuff);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, r->uvbuff);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDrawArrays(GL_TRIANGLES, 0, array_size(r->layer[layer].vertices));

    glDisable(GL_BLEND);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void renderer_text_draw(Renderer_Text *r, i32 layer)
{
    glUseProgram(r->shader.id);

    glBindBuffer(GL_ARRAY_BUFFER, r->vbuff);
    glBufferData(GL_ARRAY_BUFFER, array_size(r->layer[layer].vertices)*sizeof(Vec2f), r->layer[layer].vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, r->uvbuff);
    glBufferData(GL_ARRAY_BUFFER, array_size(r->layer[layer].uvs)*sizeof(Vec3f), r->layer[layer].uvs, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, r->vbuff);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, r->uvbuff);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDrawArrays(GL_TRIANGLES, 0, array_size(r->layer[layer].vertices));

    glDisable(GL_BLEND);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}
