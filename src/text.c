#include "text.h"

#include "image.h"
#include "glmath2.h"
#include "lib.h"
#include "shaders.h"

Font init_font(const char *filepath)
{
    Font font;
    // Initialize texture
    font.texture = load_dds(filepath);

    // Initialize VBO
    glGenBuffers(1, &font.vbuff);
    glGenBuffers(1, &font.uvbuff);

    // Initialize shader
    font.shader = load_shaders("./shader/text.vs", 0, "./shader/text.fs");

    // Initialize uniform IDs
    font.uniform = glGetUniformLocation(font.shader, "texture_sampler");

    return font;
}

void print_text(Font font, const char *text, int x, int y, int size)
{
    uint length = strlen(text);
    
    Vec2f *vertices, *uvs;
    array_init(&vertices);
    array_init(&uvs);
    
    for (uint i = 0; i < length; i++)
    {
        // Coordinates for corners of the character
        Vec2f v0 = init_vec2f(x+i*size,      y+size);
        Vec2f v1 = init_vec2f(x+i*size+size, y+size);
        Vec2f v2 = init_vec2f(x+i*size+size, y);
        Vec2f v3 = init_vec2f(x+i*size,      y);

        // Create 2 triangles forming a rectangle
        array_append(&vertices, v0);
        array_append(&vertices, v3);
        array_append(&vertices, v1);

        array_append(&vertices, v2);
        array_append(&vertices, v1);
        array_append(&vertices, v3);

        // UV coordinates for the corners of the character
        char c = text[i];
        float uv_x = (c%16)/16.0f;
        float uv_y = (c/16)/16.0f;

        Vec2f uv0 = init_vec2f(uv_x,            uv_y);
        Vec2f uv1 = init_vec2f(uv_x+1.0f/16.0f, uv_y);
        Vec2f uv2 = init_vec2f(uv_x+1.0f/16.0f, uv_y+1.0f/16.0f);
        Vec2f uv3 = init_vec2f(uv_x,            uv_y+1.0f/16.0f);

        // UV coordinates for each vertex of the triangles
        array_append(&uvs, uv0);
        array_append(&uvs, uv3);
        array_append(&uvs, uv1);

        array_append(&uvs, uv2);
        array_append(&uvs, uv1);
        array_append(&uvs, uv3);
    }

    // Fill the vertex and uv buffers
    glBindBuffer(GL_ARRAY_BUFFER, font.vbuff);
    glBufferData(GL_ARRAY_BUFFER, array_size(vertices)*sizeof(Vec2f), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, font.uvbuff);
    glBufferData(GL_ARRAY_BUFFER, array_size(uvs)*sizeof(Vec2f), uvs, GL_STATIC_DRAW);

    // Enable the text shader
    glUseProgram(font.shader);

    // Bind the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, font.texture);

    // Set the sampler to use texture unit 0
    glUniform1i(font.uniform, 0);

    // Setup vertex attrib buffer
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, font.vbuff);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // Setup uv attrib buffer
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, font.uvbuff);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // Enable transparency for font
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Draw
    glDrawArrays(GL_TRIANGLES, 0, array_size(vertices));

    // Disable transparency
    // glDisable(GL_BLEND);

    // Cleanup
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    
    array_free(vertices);
    array_free(uvs);
}

void destroy_font(Font font)
{
    glDeleteBuffers(1, &font.vbuff);
    glDeleteBuffers(1, &font.uvbuff);

    glDeleteTextures(1, &font.texture);

    glDeleteProgram(font.shader);
}
