#include "text.h"

#include "glmath2.h"
#include "lib.h"
#include "util.h"
#include "lodepng.h"
#include "config.h"

Field_Info load_msdf_metrics(const char *filepath)
{
    FileContents fc = get_file_contents(filepath);
    char *file = fc.data;

    Field_Info info = {0};

    skip_space(&file, true);
    char *dir;
    if(!parse_file_path(&file, &dir))
    {
        fprintf(stderr, "%s: Couldn't find font directory in metrics file\n", filepath);
        exit(1);
    }
    strcpy(info.name, dir);
    skip_space(&file, true);
    
    if (!parse_i32(&file, &info.glyph_count))
    {
        fprintf(stderr, "%s: Couldn't find glyph count\n", filepath);
        exit(1);
    }
    skip_space(&file, true);
    
    if (!parse_i32(&file, &info.size))
    {
        fprintf(stderr, "%s: Couldn't find font size\n", filepath);
        exit(1);
    }
    skip_space(&file, true);
    
    Glyph_Metrics m = {0};
    f32 *m_arr;
    info.ascent = -info.size;
    info.descent = info.size;
    for (int i = 0; i < info.glyph_count; i++)
    {
        m_arr = (f32*)&m;
        for (int j = 0; j < 5; j++)
        {
            if (!parse_f32(&file, &m_arr[j]))
            {
                fprintf(stderr, "%s: Couldn't parse metrics for glyph '%c'(%d)\n", filepath, i+32, j);
                exit(1);
            }
            skip_space(&file, false);
        }
        info.ascent = info.ascent > m.y1 ? info.ascent : m.y1;
        info.descent = info.descent < m.y0 ? info.descent : m.y0;
        info.metrics[i] = m;
        skip_space(&file, true);
    }
    
    return info;
}

Font load_font(const char *name)
{
    Font font = {0};

    uint error;
    u8 *image;
    uint width, height;
    u8 *png;
    size_t pngsize;
    LodePNGState state;

    glActiveTexture(GL_TEXTURE0);
    
    glGenTextures(1, &font.texture);
    glBindTexture(GL_TEXTURE_2D_ARRAY, font.texture);
    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, 96, 96, 96);

    char filepath[512];
    for (int i = 32; i < 128; i++)
    {
        image = 0;
        png = 0;
        lodepng_state_init(&state);

        snprintf(filepath, 512, "%s_msdf/%d.png", name, i);
        error = lodepng_load_file(&png, &pngsize, filepath);
        if (!error)
            error = lodepng_decode(&image, &width, &height, &state, png, pngsize);
        if (error)
        {
            fprintf(stderr, "load_msdf: ERROR %u: %s\n", error, lodepng_error_text(error));
            exit(1);
        }
    
        uint pixel_depth = 4;
    
        // Flip image
        u8 *flipped = malloc(width*height*pixel_depth);
        for (uint i = 0; i < height; i++)
            memcpy(flipped+(width*pixel_depth*(height-1-i)), image+(width*pixel_depth*i), width*pixel_depth);
        lodepng_state_cleanup(&state);
        free(image);

        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i-32, 96, 96, 1, GL_RGBA, GL_UNSIGNED_BYTE, flipped);
        free(flipped);
    }
    
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // Initialize VBO
    glGenBuffers(1, &font.vbuff);
    glGenBuffers(1, &font.uvbuff);

    // Initialize shader
    font.shader = init_shaders("./shader/text.vs", 0, "./shader/text.fs");

    // Initialize uniform IDs
    font.uniform = glGetUniformLocation(font.shader.id, "texture_sampler");

    snprintf(filepath, 512, "%s_msdfmetrics", name);
    font.info = load_msdf_metrics(filepath);
    
    return font;
}

float get_text_width(Font font, const char *text, int size)
{
    return get_text_widthn(font, text, -1, size);
}

float get_text_widthn(Font font, const char *text, int n, int size)
{
    if (!text)
        return 0;

    int len = n;
    if (len == -1)
        len = strlen(text);
    
    if (len == 0)
        return 0;
    
    float width = 0;
    float scale = (float)size / (font.info.ascent - font.info.descent);
    for (int i = 0; i < len; i++)
    {
        if (32 <= text[i] && text[i] < 128)
            width += font.info.metrics[text[i]-32].advance * scale;
    }
    return width;
}

void buffer_text(Renderer_Text *r, Font font, const char *text, float x, float y, int size, int layer)
{
    if (!text)
        return;

    if (strlen(text) == 0)
        return;
    
    float x0, x1, y0, y1;
    float ux0, ux1, uy0, uy1;

    Glyph_Metrics metrics;
    float scale = (float)size / (font.info.ascent - font.info.descent);
    while (*text)
    {
        if (32 <= *text && *text < 128)
        {
            metrics = font.info.metrics[*text-32];
            if (*text == 32) // skip spaces
            {
                x += metrics.advance*scale;
                text++;
                continue;
            }
            
            x0 = x + (metrics.x0 * scale);
            y0 = y + (metrics.y0 * scale);
            x1 = x + (metrics.x1 * scale);
            y1 = y + (metrics.y1 * scale);
            
            ux0 = metrics.x0 / font.info.size;
            uy0 = metrics.y0 / font.info.size;
            ux1 = metrics.x1 / font.info.size;
            uy1 = metrics.y1 / font.info.size;
            
            array_append(&r->layers[layer].vertices, init_vec2f(x0, y0));
            array_append(&r->layers[layer].vertices, init_vec2f(x1, y0));
            array_append(&r->layers[layer].vertices, init_vec2f(x0, y1));

            array_append(&r->layers[layer].vertices, init_vec2f(x1, y1));
            array_append(&r->layers[layer].vertices, init_vec2f(x0, y1));
            array_append(&r->layers[layer].vertices, init_vec2f(x1, y0));
            
            array_append(&r->layers[layer].uvs, init_vec3f(ux0, uy0, *text-32));
            array_append(&r->layers[layer].uvs, init_vec3f(ux1, uy0, *text-32));
            array_append(&r->layers[layer].uvs, init_vec3f(ux0, uy1, *text-32));
            
            array_append(&r->layers[layer].uvs, init_vec3f(ux1, uy1, *text-32));
            array_append(&r->layers[layer].uvs, init_vec3f(ux0, uy1, *text-32));
            array_append(&r->layers[layer].uvs, init_vec3f(ux1, uy0, *text-32));
            
            x += metrics.advance*scale;
        }
        text++;
    }
}

void destroy_font(Font font)
{
    glDeleteBuffers(1, &font.vbuff);
    glDeleteBuffers(1, &font.uvbuff);

    glDeleteTextures(1, &font.texture);

    glDeleteProgram(font.shader.id);
}
