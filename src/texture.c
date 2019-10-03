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

Texture color_texture(Vec4f color, b32 normalize)
{
    Texture t = {0};
    glGenTextures(1, &t.diffuse);
    glBindTexture(GL_TEXTURE_2D, t.diffuse);

    int scale = normalize ? 255 : 1;
    u8 c[4] = {color.r*scale, color.g*scale, color.b*scale, color.a*scale};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, c);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    t.normal = load_png("./res/normal_default.png", 0);
    t.specular = load_png("./res/specular_default.png", 0);

    t.info.width = t.info.height = 1;

    return t;
}

Texture texture_pallete(Vec4f *colors, int n, b32 normalize)
{
    Texture t = {0};
    glGenTextures(1, &t.diffuse);
    glBindTexture(GL_TEXTURE_2D, t.diffuse);

    int size = 1;
    while (n > size*size) size *= 2;
    u8 *data = calloc(size*size*4, 1);

    int scale = normalize ? 255 : 1;
    for (int i = 0; i < n; i++)
    {
        vec4f_pprint(colors[i], "COLORS");
        data[i*4+0] = scale * colors[i].r;
        data[i*4+1] = scale * colors[i].g;
        data[i*4+2] = scale * colors[i].b;
        data[i*4+3] = scale * colors[i].a;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size, size, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    t.normal = load_png("./res/normal_default.png", 0);
    t.specular = load_png("./res/specular_default.png", 0);

    t.info.width = t.info.height = size;
    
    return t;
}

Vec2f texture_pallete_index(Texture pallete, int i)
{
    int s = pallete.info.width;
    Vec2f coord = init_vec2f(i%s, (i/s));
    Vec2f uv = vec2f_scale(coord, 1/(float)s);
    return uv;
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
