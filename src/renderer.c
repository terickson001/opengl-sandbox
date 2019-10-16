#include "renderer.h"

// @Robustness(Tyler): Allow for multiple draws per layer, using different textures?
// @Robustness(Tyler): Should I just scrap this whole thing?

Renderer_2D make_renderer2d(Shader shader, Shader text_shader, Texture tex, Texture font_tex)
{
    Renderer_2D r = {0};

    r.shader = shader;
    r.text_shader = text_shader;
    r.tex = tex;
    r.font_tex = font_tex;
    
    for (int i = 0; i < RENDERER_MAX_DEPTH; i++)
    {
        array_init(&r.layers[i].vertices);
        array_init(&r.layers[i].uvs);

        array_init(&r.layers[i].text_vertices);
        array_init(&r.layers[i].text_uvs);
    }

    glGenBuffers(2, &r.vbuff);

    return r;
}

void renderer2d_begin(Renderer_2D *r)
{
    for (int i = 0; i < RENDERER_MAX_DEPTH; i++)
    {
        array_set_size(&r->layers[i].vertices, 0);
        array_set_size(&r->layers[i].uvs, 0);

        array_set_size(&r->layers[i].text_vertices, 0);
        array_set_size(&r->layers[i].text_uvs, 0);
    }
}

void renderer2d_draw_text(Renderer_2D *r, i32 layer)
{
    glBindBuffer(GL_ARRAY_BUFFER, r->vbuff);
    glBufferData(GL_ARRAY_BUFFER, array_size(r->layers[layer].text_vertices)*sizeof(Vec2f), r->layers[layer].text_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, r->uvbuff);
    glBufferData(GL_ARRAY_BUFFER, array_size(r->layers[layer].text_uvs)*sizeof(Vec3f), r->layers[layer].text_uvs, GL_STATIC_DRAW);

    glUseProgram(r->text_shader.id);
    
    glUniform2i(r->text_shader.uniforms.resolution, 1024, 768);
    glUniform1f(r->text_shader.uniforms.px_range, 4);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, r->font_tex.diffuse);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, r->vbuff);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, r->uvbuff);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays(GL_TRIANGLES, 0, array_size(r->layers[layer].text_vertices));

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void renderer2d_draw_general(Renderer_2D *r, i32 layer)
{
    glBindBuffer(GL_ARRAY_BUFFER, r->vbuff);
    glBufferData(GL_ARRAY_BUFFER, array_size(r->layers[layer].vertices)*sizeof(Vec2f), r->layers[layer].vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, r->uvbuff);
    glBufferData(GL_ARRAY_BUFFER, array_size(r->layers[layer].uvs)*sizeof(Vec2f), r->layers[layer].uvs, GL_STATIC_DRAW);

    glUseProgram(r->shader.id);
        
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, r->tex.diffuse);
    
    glUniform2i(r->shader.uniforms.resolution, 1024, 768);
    
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, r->vbuff);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, r->uvbuff);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);


    glDrawArrays(GL_TRIANGLES, 0, array_size(r->layers[layer].vertices));

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void renderer2d_draw(Renderer_2D *r)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (int i = 0; i < RENDERER_MAX_DEPTH; i++)
    {
        if (array_size(r->layers[i].vertices))
            renderer2d_draw_general(r, i);
        if (array_size(r->layers[i].text_vertices))
            renderer2d_draw_text(r, i);
    }
    glDisable(GL_BLEND);
}

