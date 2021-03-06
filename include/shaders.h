#ifndef _SHADERS_H
#define _SHADERS_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>

#include <time.h>

#include "lib.h"

typedef struct Shader
{
    GLuint id;
    time_t time;

    char *vs_filepath;
    char *gs_filepath; 
    char *fs_filepath;

    union
    {
        struct
        {
            GLint resolution;
            GLint px_range;
            
            GLint model_matrix;
            GLint view_matrix;
            GLint projection_matrix;
            GLint mvp_matrix;
            GLint vp_matrix;

            GLint diffuse_tex;
            GLint normal_tex;
            GLint specular_tex;
              
            GLint use_normal;
            GLint use_specular;
              
            GLint light_pos;
            GLint light_col;
            GLint light_pow;
        } uniforms;
        GLint uni_arr[13];
    };
} Shader;

GLuint load_shaders(const char *vertex_file_path, const char *geom_file_path, const char *fragment_file_path);
Shader init_shaders(const char *vs_filepath, const char *gs_filepath, const char *fs_filepath);
b32 shader_check_update(Shader *s);
void destroy_shaders(Shader s);

#endif // _SHADERS_H_
