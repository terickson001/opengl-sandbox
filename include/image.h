#ifndef _IMAGE_H
#define _IMAGE_H

#include <GL/glew.h>
#include <GL/gl.h>

#include "lib.h"

typedef struct TextureInfo
{
    u32 width, height;
} TextureInfo;
    
GLuint load_bmp(const char *filepath, TextureInfo *info);
GLuint load_tga(const char *filepath, TextureInfo *info);
GLuint load_dds(const char *filepath, TextureInfo *info);
GLuint load_png(const char *filepath, TextureInfo *info);

#endif // _IMAGE_H
