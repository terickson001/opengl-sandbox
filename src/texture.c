#include "texture.h"

#include <stdio.h>
#include <string.h>

#include "image.h"

const char *_get_ext(const char *filepath)
{
    const char *dot = strrchr(filepath, '.');
    return dot ? dot+1 : 0;
}

GLuint load_image(const char *filepath, TextureInfo *info)
{
    const char *ext = _get_ext(filepath);
    if (strcmp(ext, "DDS") == 0)
        return load_dds(filepath, info);
    else if (strcmp(ext, "tga") == 0)
        return load_tga(filepath, info);
    else if (strcmp(ext, "bmp") == 0)
        return load_bmp(filepath, info);
    else if (strcmp(ext, "png") == 0)
        return load_png(filepath, info);
    fprintf(stderr, "File \"%s\" is of unsupported filetype \".%s\"\n", filepath, ext);
    return 0;
}

Texture color_texture(Vec4f color)
{
    Texture t = {0};
    glGenTextures(1, &t.diffuse);
    glBindTexture(GL_TEXTURE_2D, t.diffuse);

    u8 c[4] = {color.b*255, color.g*255, color.b*255, color.a*255};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, c);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    t.normal = load_png("./res/normal_default.png", 0);
    t.specular = load_png("./res/specular_default.png", 0);
    
    return t;
}
Texture load_texture(const char *diff_path, const char *norm_path, const char *spec_path)
{
    Texture t = {0};
    
    t.diffuse  = load_image(diff_path, &t.info);
    
    if (norm_path) t.normal   = load_image(norm_path, 0);
    else           t.normal   = load_image("./res/normal_default.png", 0);
    if (spec_path) t.specular = load_image(spec_path, 0);
    else           t.specular = load_image("./res/specular_default.png", 0);
    
    return t;
}

void activate_texture(Shader s, Texture t)
{
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, t.diffuse);
        glUniform1i(s.uniforms.diffuse_tex, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, t.normal);
        glUniform1i(s.uniforms.normal_tex, 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, t.specular);
        glUniform1i(s.uniforms.specular_tex, 2);
}

void disable_texture(Shader s, Texture t)
{
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, 0);
}

void destroy_texture(Texture t)
{
    glDeleteTextures(3, &t.diffuse);
}
