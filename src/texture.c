#include "texture.h"

#include <stdio.h>
#include <string.h>

#include "image.h"

const char *_get_ext(const char *filepath)
{
    const char *dot = strrchr(filepath, '.');
    return dot ? dot+1 : 0;
}

GLuint load_image(const char *filepath)
{
    const char *ext = _get_ext(filepath);
    if (strcmp(ext, "DDS") == 0)
        return load_dds(filepath);
    else if (strcmp(ext, "tga") == 0)
        return load_tga(filepath);
    else if (strcmp(ext, "bmp") == 0)
        return load_bmp(filepath);
    else if (strcmp(ext, "png") == 0)
        return load_png(filepath);
    fprintf(stderr, "File \"%s\" is of unsupported filetype \".%s\"\n", filepath, ext);
    return 0;
}

Texture load_texture(const char *diff_path, const char *norm_path, const char *spec_path)
{
    Texture t = {0};
    
    t.diffuse  = load_image(diff_path);
    
    if (norm_path) t.normal   = load_image(norm_path);
    else           t.normal   = load_image("./res/normal_default.png");
    if (spec_path) t.specular = load_image(spec_path);
    else           t.specular = load_image("./res/specular_default.png");
    
    return t;
}

void activate_texture(Shader s, Texture t)
{
    if (t.diffuse)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, t.diffuse);
        glUniform1i(s.uniforms.diffuse_tex, 0);
    }

    if (t.normal)
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, t.normal);
        glUniform1i(s.uniforms.normal_tex, 1);
        glUniform1i(s.uniforms.use_normal, 1);
    }

    if (t.specular)
    {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, t.specular);
        glUniform1i(s.uniforms.specular_tex, 2);
        glUniform1i(s.uniforms.use_specular, 1);
    }
}

void disable_texture(Shader s, Texture t)
{
    if (t.diffuse)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    if (t.normal)
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
        glUniform1i(s.uniforms.use_normal, 0);
    }

    if (t.specular)
    {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, 0);
        glUniform1i(s.uniforms.use_specular, 0);
    }
}

void destroy_texture(Texture t)
{
    glDeleteTextures(3, &t.diffuse);
}
