#ifndef _VAO_H
#define _VAO_H

#include <GL/glew.h>
#include <GL/gl.h>

#include "lib.h"

typedef struct VAO
{
    GLuint id;

    GLuint vbuff;
    GLuint uvbuff;
    GLuint nbuff;
    GLuint tbuff;
    GLuint btbuff;
    GLuint ebuff;

    uint vertex_offset;
    uint element_offset;
} VAO;

VAO make_vao();

#endif // _VAO_H
