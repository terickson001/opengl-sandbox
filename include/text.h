#ifndef _TEXT_H
#define _TEXT_H

#include <GL/glew.h>
#include <GL/gl.h>

#include "shaders.h"
#include "image.h"

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
    GLuint uniform;
    GLuint texture;

    Field_Info info;
} Font;
    
Font load_font(const char *directory);
float get_text_width(Font font, const char *text, int size);
void print_text(Font font, const char *text, int x, int y, int size, int layer);
void destroy_font(Font font);

#endif // _TEXT_H
