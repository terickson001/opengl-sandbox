#ifndef _MODEL_H
#define _MODEL_H

#include <GL/glew.h>
#include <GL/gl.h>

#include "glmath2.h"
#include "lib.h"
#include "shaders.h"

typedef struct Model
{
    Vec3f *vertices;
    Vec2f *uvs;
    Vec3f *normals;
    Vec3f *tangents;
    Vec3f *bitangents;
    
    b32 indexed;
    u16 *indices;

    GLuint vao;
    
    GLuint vbuff;
    GLuint uvbuff;
    GLuint nbuff;
    GLuint tbuff;
    GLuint btbuff;
    GLuint ebuff;
} Model;


Model load_obj(const char *filepath);
void index_model(Model *m);
void compute_tangent_basis(Model *m);
void create_model_vbos(Model *m);
void invert_uvs(Model *m);
Model make_model(char const *filepath, bool normals, bool invert_uv);

void draw_model_normals(Model m, Mat4f mvp);
void draw_model(Shader s, Model m);
void free_model(Model *m);
void destroy_model(Model *m);

#endif // _MODEL_H
