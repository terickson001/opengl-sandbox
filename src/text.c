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
    float width = 0;
    float scale = (float)size / (font.info.ascent - font.info.descent);
    while (*text)
    {
        if (32 <= *text && *text < 128)
            width += font.info.metrics[*text-32].advance * scale;
        text++;
    }
    return width;
}

void print_text(Font font, const char *text, int x, int y, int size, int layer)
{
    glEnable(GL_TEXTURE_2D_ARRAY);
    glBindTexture(GL_TEXTURE_2D_ARRAY, font.texture);
    
    Vec3f *vertices;
    Vec3f *uvs;
    array_init(&vertices);
    array_init(&uvs);

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
            
            x0 = x + metrics.x0*scale;
            y0 = y + metrics.y0*scale;
            x1 = x + metrics.x1*scale;
            y1 = y + metrics.y1*scale;
            
            ux0 = (metrics.x0)/font.info.size;
            uy0 = (metrics.y0)/font.info.size;
            ux1 = (metrics.x1)/font.info.size;
            uy1 = (metrics.y1)/font.info.size;
            
            array_append(&vertices, init_vec3f(x0, y0, layer));
            array_append(&vertices, init_vec3f(x1, y0, layer));
            array_append(&vertices, init_vec3f(x0, y1, layer));

            array_append(&vertices, init_vec3f(x1, y1, layer));
            array_append(&vertices, init_vec3f(x0, y1, layer));
            array_append(&vertices, init_vec3f(x1, y0, layer));
            
            array_append(&uvs, init_vec3f(ux0, uy0, *text-32));
            array_append(&uvs, init_vec3f(ux1, uy0, *text-32));
            array_append(&uvs, init_vec3f(ux0, uy1, *text-32));
            
            array_append(&uvs, init_vec3f(ux1, uy1, *text-32));
            array_append(&uvs, init_vec3f(ux0, uy1, *text-32));
            array_append(&uvs, init_vec3f(ux1, uy0, *text-32));
            
            x += metrics.advance*scale;
        }
        text++;
    }

    // Fill the vertex and uv buffers
    glBindBuffer(GL_ARRAY_BUFFER, font.vbuff);
    glBufferData(GL_ARRAY_BUFFER, array_size(vertices)*sizeof(Vec3f), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, font.uvbuff);
    glBufferData(GL_ARRAY_BUFFER, array_size(uvs)*sizeof(Vec3f), uvs, GL_STATIC_DRAW);

    // Enable the text shader
    glUseProgram(font.shader.id);

    glUniform2i(font.shader.uniforms.resolution, 1024, 768);
    // Bind the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, font.texture);

    // Set the sampler to use texture unit 0
    glUniform1i(font.uniform, 0);

    // Setup vertex attrib buffer
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, font.vbuff);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Setup uv attrib buffer
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, font.uvbuff);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Enable transparency for font
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Draw
    glDrawArrays(GL_TRIANGLES, 0, array_size(vertices));

    // Disable transparency
    glDisable(GL_BLEND);

    // Cleanup
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    
    array_free(vertices);
    array_free(uvs);
}


void destroy_font(Font font)
{
    glDeleteBuffers(1, &font.vbuff);
    glDeleteBuffers(1, &font.uvbuff);

    glDeleteTextures(1, &font.texture);

    glDeleteProgram(font.shader.id);
}
