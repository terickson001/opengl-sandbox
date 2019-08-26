#include "vao.h"

VAO make_vao()
{
    VAO v;

    glGenVertexArrays(1, &v.id);
    glBindVertexArray(v.id);

    glGenBuffers(1, &v.vbuff);
    glBindBuffer(GL_ARRAY_BUFFER, v.vbuff);

    glGenBuffers(1, &v.uvbuff);
    glBindBuffer(GL_ARRAY_BUFFER, v.uvbuff);

    glGenBuffers(1, &v.nbuff);
    glBindBuffer(GL_ARRAY_BUFFER, v.nbuff);

    glGenBuffers(1, &v.tbuff);
    glBindBuffer(GL_ARRAY_BUFFER, v.tbuff);

    glGenBuffers(1, &v.btbuff);
    glBindBuffer(GL_ARRAY_BUFFER, v.btbuff);
    
    glGenBuffers(1, &v.ebuff);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, v.ebuff);

    v.vertex_offset = 0;
    v.element_offset = 0;
    return v;
}
