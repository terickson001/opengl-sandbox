#ifndef _IMAGE_H
#define _IMAGE_H

#include <GL/glew.h>
#include <GL/gl.h>

GLuint load_bmp(const char *filepath);
GLuint load_tga(const char *filepath);
GLuint load_dds(const char *filepath);

#endif // _IMAGE_H
