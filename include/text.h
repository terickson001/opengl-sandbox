#ifndef _TEXT_H
#define _TEXT_H

#include <GL/glew.h>
#include <GL/gl.h>

typedef struct Font
{
    GLuint texture;
    GLuint vbuff, uvbuff;
    GLuint shader;
    GLuint uniform;
} Font;

Font init_font(const char *filepath);
void print_text(Font font, const char *text, int x, int y, int size);
void destroy_font(Font font);

#endif // _TEXT_H
