#include "image.h"

#include <math.h>

#include "lodepng.h"

GLuint load_bmp(const char *filepath, TextureInfo *info)
{
    byte header[54];
    uint data_pos;
    uint width, height;
    uint image_size;

    FILE *file = fopen(filepath, "rb");
    if (!file)
    {
        fprintf(stderr, "Image \"%s\" could not be opened\n", filepath);
        return 0;
    }

    if (fread(header, 1, 54, file) != 54 ||
        header[0] != 'B' || header[1] != 'M')
    {
        fprintf(stderr, "File \"%s\" is not a valid BMP\n", filepath);
        return 0;
    }

    data_pos   = *(int*)&(header[0x0A]);
    image_size = *(int*)&(header[0x22]);
    width      = *(int*)&(header[0x12]);
    height     = *(int*)&(header[0x16]);

    if (!image_size) image_size = width*height*3;
    if (!data_pos)   data_pos = 54;

    byte *data = malloc(image_size * sizeof(byte));
    fread(data, 1, image_size, file);
    fclose(file);

    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
    free(data);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    if (info)
    {
        info->width = width;
        info->height = height;
    }
    
    return texture_id;
}

GLuint load_tga(const char *filepath, TextureInfo *info)
{
    byte header[18];

    u8 id_length;
    u16 width, height;
    u8 cmap_type;
    u8 image_type;
    u16 cmap_start, cmap_len;
    u8 cmap_depth;
    u8 pixel_depth;
    u8 image_descriptor;
    
    FILE *file = fopen(filepath, "rb");
    if (!file)
    {
        fprintf(stderr, "Image \"%s\" could not be opened\n", filepath);
        return 0;
    }

    if (fread(header, 1, 18, file) != 18)
    {
        fprintf(stderr, "Image \"%s\" is not a valid TGA\n", filepath);
        return 0;
    }

    id_length        = header[0x00];
    cmap_type        = header[0x01];
    image_type       = header[0x02];
    cmap_start       = *(u16 *)&(header[0x03]);
    cmap_len         = *(u16 *)&(header[0x05]);
    cmap_depth       = header[0x07];
    width            = *(u16 *)&(header[0x0C]);
    height           = *(u16 *)&(header[0x0E]);
    pixel_depth      = header[0x10];
    image_descriptor = header[0x11];

    b32 RLE = image_type & 0b1000;
    
    printf("======= Load TGA (%s) =======\n", filepath);
    printf("cmap_type: %hhu\n", cmap_type);
    printf("cmap_start: %hu\n", cmap_start);
    printf("cmap_len: %hu\n", cmap_len);
    printf("cmap_depth: %hhu\n", cmap_depth);

    printf("id_length: %hhu\n", id_length);
    printf("image_type: %hhu\n", image_type);
    printf("  RLE?: %s\n", RLE?"Yes":"No");
    printf("width: %hu\n", width);
    printf("height: %hu\n", height);
    printf("pixel_depth: %hhu\n", pixel_depth);
    printf("image_descriptor: %hhu\n", image_descriptor);
    printf("========================\n");

    uint pixel_depth_bytes = ceil((float)pixel_depth/8);
    uint cmap_depth_bytes = ceil((float)cmap_depth/8);
    byte image_id[id_length];
    if (id_length &&
        fread(image_id, 1, id_length, file) != id_length)
    {
        fprintf(stderr, "Could not read Image ID in TGA \"%s\"\n", filepath);
        return 0;
    }

    byte cmap_data[cmap_len*cmap_depth_bytes];
    if (cmap_type &&
        fread(cmap_data, 1, cmap_len*cmap_depth_bytes, file) != cmap_len*cmap_depth_bytes)
    {
        fprintf(stderr, "Could not read Colormap in TGA \"%s\"\n", filepath);
        return 0;
    }

    byte *raw_image_data = malloc(width*height*pixel_depth_bytes);
    int image_size;
    if ((image_size = fread(raw_image_data, 1, width*height*pixel_depth_bytes, file)) == 0)
    {
        fprintf(stderr, "Could not read Image Data in TGA \"%s\"\n", filepath);
        return 0;
    }

    byte *image_data = raw_image_data;
    int decoded_data_size = width*height*pixel_depth_bytes;
        
    int image_data_size;
    if (cmap_type)
        image_data_size = width*height*cmap_depth_bytes;
    else
        image_data_size = decoded_data_size;

    GLuint texture_id;
    glGenTextures(1, &texture_id);

    glBindTexture(GL_TEXTURE_2D, texture_id);

    byte *decoded_image_data = 0;
    if (RLE)
    {
        decoded_image_data = malloc(decoded_data_size);
        int pixel_count = 0;
        int i = 0;
        int decoded_index = 0;
        while (pixel_count < width*height)
        {
            byte count = image_data[i++];
            b32 encoded = count & 0b10000000;
            count &= 0b01111111;
            if (encoded)
            {
                for (int j = 0; j < count+1; j++)
                {
                    memcpy(decoded_image_data+decoded_index, image_data+i, pixel_depth_bytes);
                    decoded_index += pixel_depth_bytes;
                    pixel_count++;
                }
                i += pixel_depth_bytes;
            }
            else
            {
                for (int j = 0; j < count+1; j++)
                {
                    memcpy(decoded_image_data+decoded_index, image_data+i, pixel_depth_bytes);
                    i += pixel_depth_bytes;
                    decoded_index += pixel_depth_bytes;
                    pixel_count++;
                }
            }
        }
        free(image_data);
        image_data = decoded_image_data;
    }

    byte *colormapped_image_data = malloc(image_data_size);
    u8 result_depth = pixel_depth;
    if (cmap_type)
    {
        int colormapped_index = 0;
        for (int i = 0; i < width*height; i++)
        {
            memcpy(colormapped_image_data+colormapped_index,
                   cmap_data+(image_data[i*pixel_depth_bytes]*cmap_depth_bytes),
                   cmap_depth_bytes);
            colormapped_index += cmap_depth_bytes;
        }
        free(image_data);
        image_data = colormapped_image_data;
        result_depth = cmap_depth;
    }

    if (result_depth == 15)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB5,
                     width, height, 0, GL_BGR,
                     GL_UNSIGNED_SHORT_1_5_5_5_REV, image_data);
    else if (result_depth == 16)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB5_A1,
                     width, height, 0, GL_BGRA,
                     GL_UNSIGNED_SHORT_1_5_5_5_REV, image_data);
    else if (result_depth == 24)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8,
                     width, height, 0, GL_BGR,
                     GL_UNSIGNED_BYTE, image_data);
    else if (result_depth == 32)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
                     width, height, 0, GL_BGRA,
                     GL_UNSIGNED_BYTE, image_data);

    free(image_data);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    if (info)
    {
        info->width = width;
        info->height = height;
    }
    
    return texture_id;
}

// NOTE: V-coordinate of UVs must be inversed
GLuint load_dds(const char *filepath, TextureInfo *info)
{
    byte header[124];
    FILE *file;

    file = fopen(filepath, "rb");
    if (!file)
    {
        fprintf(stderr, "Could not open image \"%s\"\n", filepath);
        return 0;
    }

    char filecode[4];
    fread(filecode, 1, 4, file);
    if (strncmp(filecode, "DDS ", 4) != 0)
    {
        fprintf(stderr, "Image \"%s\" is not a valid DDS\n", filepath);
        fclose(file);
        return 0;
    }

    fread(header, 1, 124, file);

    uint height       = *(uint *)&(header[0x08]);
    uint width        = *(uint *)&(header[0x0C]);
    uint linear_size  = *(uint *)&(header[0x10]);
    uint mipmap_count = *(uint *)&(header[0x18]);
    uint four_cc      = *(uint *)&(header[0x50]);
    
    uint bufsize = mipmap_count > 1 ? linear_size * 2 : linear_size;
    byte *buf = malloc(bufsize);
    fread(buf, 1, bufsize, file);

    fclose(file);
    
#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII
    
    uint format;
    switch (four_cc)
    {
    case FOURCC_DXT1:
        format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        break;
    case FOURCC_DXT3:
        format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        break;
    case FOURCC_DXT5:
        format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        break;
    default:
        return 0;
    }

    GLuint texture_id;
    glGenTextures(1, &texture_id);

    glBindTexture(GL_TEXTURE_2D, texture_id);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    uint block_size = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
    uint offset = 0;

	for (unsigned int level = 0; level < mipmap_count && (width || height); ++level) 
	{ 
		unsigned int size = ((width+3)/4)*((height+3)/4)*block_size; 
		glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,  
			0, size, buf + offset); 
	 
		offset += size; 
		width  /= 2; 
		height /= 2; 

		if (width  < 1) width  = 1;
		if (height < 1) height = 1;
	}

    free(buf);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    if (info)
    {
        info->width = width;
        info->height = height;
    }
    return texture_id;
}

GLuint load_png(const char *filepath, TextureInfo *info)
{
    uint error;
    u8 *image = 0;
    uint width, height;
    u8 *png = 0;
    size_t pngsize;
    LodePNGState state;

    lodepng_state_init(&state);

    
    error = lodepng_load_file(&png, &pngsize, filepath);
    if (!error)
        error = lodepng_decode(&image, &width, &height, &state, png, pngsize);
    if (error)
    {
        fprintf(stderr, "load_png: ERROR %u: %s\n", error, lodepng_error_text(error));
        exit(1);
    }
    
    // printf("PNG LOADED: %s\n  width: %hd\n  height: %hd\n", filepath, width, height);
    uint pixel_depth = 4;
    
    // Flip image
    u8 *flipped = malloc(width*height*pixel_depth);
    for (uint i = 0; i < height; i++)
        memcpy(flipped+(width*pixel_depth*(height-1-i)), image+(width*pixel_depth*i), width*pixel_depth);
    lodepng_state_cleanup(&state);
    free(image);

    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, flipped);
    free(flipped);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glGenerateMipmap(GL_TEXTURE_2D);

    if (info)
    {
        info->width = width;
        info->height = height;
    }
    
    return texture_id;
}

#if 0
// THIS IS A WIP

#define PNG_SIGNATURE  0x89504E470D0A1A0A

// Critical Chunks
#define PNG_CHUNK_IHDR 0x49484452
#define PNG_CHUNK_IDAT 0x4844415E
#define PNG_CHUNK_IEND 0x48454E44
#define PNG_CHUNK_PLTE 0x504C5445

// Ancillary Chunks
#define PNG_CHUNK_tRNS 0x74524E53
#define PNG_CHUNK_cHRM 0x6348524D
#define PNG_CHUNK_gAMA 0x67414D41
#define PNG_CHUNK_iCCP 0x69434350
#define PNG_CHUNK_sBIT 0x73424954
#define PNG_CHUNK_sRGB 0x73524742

#define PNG_CHUNK_tEXt 0x74455874
#define PNG_CHUNK_zTXt 0x7A545874
#define PNG_CHUNK_iTXt 0x69545874

#define PNG_CHUNK_bKGD 0x624B4744
#define PNG_CHUNK_hIST 0x68495354
#define PNG_CHUNK_pHYs 0x70485973
#define PNG_CHUNK_sPLT 0x73504c54
#define PNG_CHUNK_tIME 0x74494D45

enum PNG_ColorType
{
    PNG_GREYSCALE    = 0,
    PNG_TRUECOLOR    = 2,
    PNG_INDEXEDCOLOR = 3,
    PNG_GREYSCALE_A  = 4,
    PNG_TRUECOLOR_A  = 6,
};

GLuint load_png(const char *filepath)
{
    FILE *file = fopen(filepath, "rb");
    if (!file)
    {
        fprintf(stderr, "Image \"%s\" could not be opened\n", filepath);
        return 0;
    }

    byte signature[8];

    if (fread(signature, 1, 8, file) != 8 || *(u64 *)signature != PNG_SIGNATURE)
    {
        fprintf(stderr, "Image \"%s\" is not a valid PNG\n", filepath);
        return 0;
    }

    u32 chunk_size;
    u32 chunk_type;
    u32 crc;
    byte *chunk_dat;
    
    fread(&chunk_size, 1, 4, file);
    fread(&chunk_type, 1, 4, file);
    chunk_dat = malloc(chunk_size);
    fread(&chunk_dat, 1, chunk_size, file);
    fread(&crc, 1, 4, file);
    
    u32 width           = *(u32 *)&(chunk_dat[0x00]);
    u32 height          = *(u32 *)&(chunk_dat[0x04]);
    u8 depth            = chunk_dat[0x08];
    u8 color_type       = chunk_dat[0x09];
    u8 compression_type = chunk_dat[0x0A];
    u8 filter_type      = chunk_dat[0x0B];
    u8 interlace_type   = chunk_dat[0x0C];
    free(chunk_dat);
    
    switch (color_type)
    {
    case PNG_GREYSCALE:
        if (!(depth & 0x1F))
        {
            fprintf(stderr, "Greyscale PNG cannot have a color depth of %d\n", depth);
            return 0;
        }
        break;
    case PNG_TRUECOLOR:
        if (!(depth & 0x18))
        {
            fprintf(stderr, "Truecolor PNG cannot have a color depth of %d\n", depth);
            return 0;
        }
        break;
    case PNG_INDEXED_COLOR:
        if (!(depth & 0x0F))
        {
            fprintf(stderr, "Indexed Color PNG cannot have a color depth of %d\n", depth);
            return 0;
        }
        break;
    case PNG_GREYSCALE_A:
        if (!(depth & 0x18))
        {
            fprintf(stderr, "Greyscale PNG w/ alpha cannot have a color depth of %d\n", depth);
            return 0;
        }
        break;
    case PNG_TRUECOLOR_A:
        if (!(depth & 0x18))
        {
            fprintf(stderr, "Truecolor PNG w/ alpha cannot have a color depth of %d\n", depth);
            return 0;
        }
        break;
    default: break;
    }

    fread(&chunk_size, 1, 4, file);
    fread(&chunk_type, 1, 4, file);
    chunk_dat = malloc(chunk_size);
    fread(&chunk_dat, 1, chunk_size, file);
    fread(&crc, 1, 4, file);

    switch (chunk_type)
    {
    case PNG_CHUNK_tIME:
    case PNG_CHUNK_zTXt:
    case PNG_CHUNK_tEXt:
    case PNG_CHUNK_iTXt:
    case PNG_CHUNK_pHYs:
    case PNG_CHUNK_sPLT:
    case PNG_CHUNK_iCCP:
    case PNG_CHUNK_sRGB:
    case PNG_CHUNK_sBIT:
    case PNG_CHUNK_gAMA:
    case PNG_CHUNK_cHRM:
        
    case PNG_CHUNK_PLTE:
        
    case PNG_CHUNK_tRNS:
    case PNG_CHUNK_hIST:
    case PNG_CHUNK_bKGD:
        
    case PNG_CHUNK_IDAT:
    }
}
#endif
