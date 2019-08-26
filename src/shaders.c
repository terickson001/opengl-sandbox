#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "shaders.h"

#include "util.h"

GLuint load_shaders(const char *vertex_file_path, const char *geom_file_path, const char *fragment_file_path)
{
    // Create the shaders
    GLuint vs_id = glCreateShader(GL_VERTEX_SHADER);
    GLuint gs_id = 0;
    if (geom_file_path)
        gs_id = glCreateShader(GL_GEOMETRY_SHADER);
    GLuint fs_id = glCreateShader(GL_FRAGMENT_SHADER);

    // Read the vertex shader code from the file
    char *vs_code, *fs_code, *gs_code;
    if (!(vs_code = load_file(vertex_file_path)))
    {
        fprintf(stderr, "Failed to open vertex shader '%s'\n", vertex_file_path);
        return 0;
    }
    if (!(fs_code = load_file(fragment_file_path)))
    {
        fprintf(stderr, "Failed to open fragment shader '%s'\n", fragment_file_path);
        return 0;
    }
    if (geom_file_path && !(gs_code = load_file(geom_file_path)))
    {
        fprintf(stderr, "Failed to open geometry shader '%s'\n", geom_file_path);
        return 0;
    }

    GLint result = GL_FALSE;
    int info_log_length;

    // Compile vertex shader
    printf("Compiling shader: %s\n", vertex_file_path);
    char const *vs_source_pointer = vs_code;
    glShaderSource(vs_id, 1, &vs_source_pointer, 0);
    glCompileShader(vs_id);

    // Check vertex shader
    glGetShaderiv(vs_id, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vs_id, GL_INFO_LOG_LENGTH, &info_log_length);
    if (info_log_length > 0)
    {
        char vs_err_msg[info_log_length+1];
        glGetShaderInfoLog(vs_id, info_log_length, 0, vs_err_msg);
        printf("%s\n", vs_err_msg);
    }

    // Compile fragment shader
    printf("Compiling shader: %s\n", fragment_file_path);
    char const *fs_source_pointer = fs_code;
    glShaderSource(fs_id, 1, &fs_source_pointer, 0);
    glCompileShader(fs_id);

    // Check fragment shader
    glGetShaderiv(fs_id, GL_COMPILE_STATUS, &result);
    glGetShaderiv(fs_id, GL_INFO_LOG_LENGTH, &info_log_length);
    if (info_log_length > 0)
    {
        char fs_err_msg[info_log_length+1];
        glGetShaderInfoLog(fs_id, info_log_length, 0, fs_err_msg);
        printf("%s\n", fs_err_msg);
    }

    if (geom_file_path)
    {
        // Compile geometry shader
        printf("Compiling shader: %s\n", geom_file_path);
        char const *gs_source_pointer = gs_code;
        glShaderSource(gs_id, 1, &gs_source_pointer, 0);
        glCompileShader(gs_id);

        // Check geometry shader
        glGetShaderiv(gs_id, GL_COMPILE_STATUS, &result);
        glGetShaderiv(gs_id, GL_INFO_LOG_LENGTH, &info_log_length);
        if (info_log_length > 0)
        {
            char gs_err_msg[info_log_length+1];
            glGetShaderInfoLog(gs_id, info_log_length, 0, gs_err_msg);
            printf("%s\n", gs_err_msg);
        }
    }
    
    // Link the program
    printf("Linking program\n");
    GLuint program_id = glCreateProgram();
    glAttachShader(program_id, vs_id);
    if (geom_file_path) glAttachShader(program_id, gs_id);
    glAttachShader(program_id, fs_id);
    glLinkProgram(program_id);

    //Check the program
    glGetProgramiv(program_id, GL_LINK_STATUS, &result);
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);
    if (info_log_length > 0)
    {
        char program_err_msg[info_log_length+1];
        glGetProgramInfoLog(program_id, info_log_length, 0, program_err_msg);
        printf("%s\n", program_err_msg);
    }

    glDetachShader(program_id, vs_id);
    glDetachShader(program_id, fs_id);
    if (geom_file_path) glDetachShader(program_id, gs_id);

    glDeleteShader(vs_id);
    glDeleteShader(fs_id);
    if (geom_file_path) glDeleteShader(gs_id);

    free(vs_code);
    free(fs_code);
    if (geom_file_path) free(gs_code);
    
    return program_id;
}

Shader init_shaders(const char *vs_filepath, const char *gs_filepath, const char *fs_filepath)
{
    Shader s;

    s.id = load_shaders(vs_filepath, gs_filepath, fs_filepath);

    int vs_len = strlen(vs_filepath);
    s.vs_filepath = malloc(vs_len+1);
    memcpy(s.vs_filepath, vs_filepath, vs_len+1);

    s.gs_filepath = 0;
    if (gs_filepath)
    {
        int gs_len = strlen(gs_filepath);
        s.gs_filepath = malloc(gs_len+1);
        memcpy(s.gs_filepath, gs_filepath, gs_len+1);
    }
    
    int fs_len = strlen(fs_filepath);
    s.fs_filepath = malloc(fs_len+1);
    memcpy(s.fs_filepath, fs_filepath, fs_len+1);

    s.uniforms.model_matrix      = glGetUniformLocation(s.id, "M");
    s.uniforms.view_matrix       = glGetUniformLocation(s.id, "V");
    s.uniforms.projection_matrix = glGetUniformLocation(s.id, "P");
    s.uniforms.mvp_matrix        = glGetUniformLocation(s.id, "MVP");
    s.uniforms.vp_matrix         = glGetUniformLocation(s.id, "VP");
    
    s.uniforms.diffuse_tex  = glGetUniformLocation(s.id, "diffuse_sampler");
    s.uniforms.normal_tex   = glGetUniformLocation(s.id, "normal_sampler");
    s.uniforms.specular_tex = glGetUniformLocation(s.id, "specular_sampler");

    s.uniforms.use_normal   = glGetUniformLocation(s.id, "use_normal");
    s.uniforms.use_specular = glGetUniformLocation(s.id, "use_specular");

    s.uniforms.light_pos = glGetUniformLocation(s.id, "light_position_m");
    s.uniforms.light_col = glGetUniformLocation(s.id, "light_color");
    s.uniforms.light_pow = glGetUniformLocation(s.id, "light_power");

    struct stat st;
    stat(vs_filepath, &st);
    s.time = st.st_mtim.tv_sec;
    if (gs_filepath)
    {
        stat(gs_filepath, &st);
        s.time = st.st_mtim.tv_sec > s.time ? st.st_mtim.tv_sec : s.time;
    }
    stat(fs_filepath, &st);
    s.time = st.st_mtim.tv_sec > s.time ? st.st_mtim.tv_sec : s.time;
    
    return s;
}

b32 shader_check_update(Shader *s)
{
    time_t new_time;
    struct stat st;
    stat(s->vs_filepath, &st);
    new_time = st.st_mtim.tv_sec;
    if (s->gs_filepath)
    {
        stat(s->gs_filepath, &st);
        new_time = st.st_mtim.tv_sec > new_time ? st.st_mtim.tv_sec : new_time;
    }
    stat(s->fs_filepath, &st);
    new_time = st.st_mtim.tv_sec > new_time ? st.st_mtim.tv_sec : new_time;

    if (new_time > s->time)
    {
        Shader old = *s;
        *s = init_shaders(s->vs_filepath, s->gs_filepath, s->fs_filepath);
        destroy_shaders(old);
        return true;
    }
    return false;
}

void destroy_shaders(Shader s)
{
    free(s.vs_filepath);
    free(s.fs_filepath);

    glDeleteProgram(s.id);
}
