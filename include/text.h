#ifndef _TEXT_H
#define _TEXT_H

#include <GL/glew.h>
#include <GL/gl.h>

#include "shaders.h"
#include "image.h"
#include "renderer.h"

typedef struct Glyph_Metrics
{
    f32 x0, y0, x1, y1;
    f32 advance;
} Glyph_Metrics;

typedef struct Field_Info
{
    char name[64];
    int glyph_count;
    int size;
    float ascent, descent;
    
    Glyph_Metrics metrics[96];
} Field_Info;

typedef struct Font
{
    GLuint vbuff, uvbuff;
    Shader shader;
    // GLuint uniform;
    Texture texture;

    Field_Info info;
} Font;
    
Font load_font(const char *directory);
float get_text_width(Font font, const char *text, int size);
float get_text_widthn(Font font, const char *text, int n, int size);
void buffer_text(Renderer_2D *r, Font font, const char *text, float x, float y, int size, int layer);
void destroy_font(Font font);

#endif // _TEXT_H
