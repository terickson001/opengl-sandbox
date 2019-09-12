#include "cube_march.h"

#include <time.h>
#include <stdlib.h>

int field_index(Vec3f pos, Vec3f res)
{
    return pos.z*res.y*res.x + pos.y*res.x + pos.x;
}

Vec3f midpoint(Vec4f a, Vec4f b)
{
    return init_vec3f((a.x+b.x)/2, (a.y+b.y)/2, (a.z+b.z)/2);
}

Vec3f homogenize(Vec3f p, Vec3f res)
{
    Vec3f half_res = vec3f_scale(res, 0.5);
    Vec3f result = vec3f_div(vec3f_sub(p, half_res), half_res);
    return result;
}

Vec3f *gen_gradients(Vec3f res)
{
    res = vec3f_add(res, init_vec3f(1, 1, 1));
    srand(time(0));
    Vec3f *gradients = malloc(res.x*res.y*res.z*sizeof(Vec3f));
    for (int i = 0; i < res.x*res.y*res.z; i++)
    {
        gradients[i].x = (rand() / (RAND_MAX/2.0f))-1.0f;
        gradients[i].y = (rand() / (RAND_MAX/2.0f))-1.0f;
        gradients[i].z = (rand() / (RAND_MAX/2.0f))-1.0f;
        gradients[i] = vec3f_normalize(gradients[i]);
        vec3f_pprint(gradients[i], "GRADIENT");
    }
    return gradients;
}

float lerp(float a, float b, float w)
{
    return (1.0-w)*a + w*b;
}

float dotGridGradient(int ix, int iy, int iz, float x, float y, float z, Vec3f *gradients, Vec3f res)
{
    float dx = x - (float)ix;
    float dy = y - (float)iy;
    float dz = z - (float)iz;

    Vec3f gradient = gradients[(int)(iz*res.y*res.x + iy*res.x + ix)];
    return (dx*gradient.x + dy*gradient.y + dz*gradient.z);
}

void perlin_noise(Vec4f *pos, Vec3f res, Vec3f *gradients)
{
    // Determine grid cell coordinates
    int x0 = (int)pos->x;
    int x1 = x0 + 1;
    int y0 = (int)pos->y;
    int y1 = y0 + 1;
    int z0 = (int)pos->z;
    int z1 = z0 + 1;

    // Determine interpolation weights
    // Could also use higher order polynomial/s-curve here
    float sx = pos->x - (float)x0;
    float sy = pos->y - (float)y0;
    float sz = pos->z - (float)z0;
    // Interpolate between grid point gradients
    float n0, n1, ix0, ix1, iy0, iy1;

    
    n0 = dotGridGradient(x0, y0, z0, pos->x, pos->y, pos->z, gradients, res);
    n1 = dotGridGradient(x1, y0, z0, pos->x, pos->y, pos->z, gradients, res);
    ix0 = lerp(n0, n1, sx);

    n0 = dotGridGradient(x0, y1, z0, pos->x, pos->y, pos->z, gradients, res);
    n1 = dotGridGradient(x1, y1, z0, pos->x, pos->y, pos->z, gradients, res);
    ix1 = lerp(n0, n1, sx);

    iy0 = lerp(ix0, ix1, sy);

    n0 = dotGridGradient(x0, y0, z1, pos->x, pos->y, pos->z, gradients, res);
    n1 = dotGridGradient(x1, y0, z1, pos->x, pos->y, pos->z, gradients, res);
    ix0 = lerp(n0, n1, sx);

    n0 = dotGridGradient(x0, y1, z1, pos->x, pos->y, pos->z, gradients, res);
    n1 = dotGridGradient(x1, y1, z1, pos->x, pos->y, pos->z, gradients, res);
    ix1 = lerp(n0, n1, sx);

    iy1 = lerp(ix0, ix1, sy);

    pos->w = lerp(iy0, iy1, sz);
}

Model cube_march_mesh(Vec4f *field, Vec3f res, float threshold)
{
    Model m = {0};
    array_init(&m.vertices);
    array_init(&m.uvs);
    array_init(&m.normals);
    for (int z = 0; z < res.z-1; z++)
    {
        for (int y = 0; y < res.y-1; y++)
        {
            for (int x = 0; x < res.x-1; x++)
            {
                Vec4f cube_corners[8] = {
                    field[field_index(init_vec3f(x,   y,   z),   res)],
                    field[field_index(init_vec3f(x+1, y,   z),   res)],
                    field[field_index(init_vec3f(x+1, y,   z+1), res)],
                    field[field_index(init_vec3f(x,   y,   z+1), res)],
                    field[field_index(init_vec3f(x,   y+1, z),   res)],
                    field[field_index(init_vec3f(x+1, y+1, z),   res)],
                    field[field_index(init_vec3f(x+1, y+1, z+1), res)],
                    field[field_index(init_vec3f(x,   y+1, z+1), res)],
                };

                int variation_index = 0;
                if (cube_corners[0].w < threshold) variation_index |= 1;
                if (cube_corners[1].w < threshold) variation_index |= 2;
                if (cube_corners[2].w < threshold) variation_index |= 4;
                if (cube_corners[3].w < threshold) variation_index |= 8;
                if (cube_corners[4].w < threshold) variation_index |= 16;
                if (cube_corners[5].w < threshold) variation_index |= 32;
                if (cube_corners[6].w < threshold) variation_index |= 64;
                if (cube_corners[7].w < threshold) variation_index |= 128;

                const int *tris = triangulation[variation_index];
                for (int i = 0; tris[i] != -1; i+=3)
                {
                    int a0 = edge_corner_A[tris[i]];
                    int b0 = edge_corner_B[tris[i]];

                    int a1 = edge_corner_A[tris[i+1]];
                    int b1 = edge_corner_B[tris[i+1]];

                    int a2 = edge_corner_A[tris[i+2]];
                    int b2 = edge_corner_B[tris[i+2]];

                    Vec3f vert0 = midpoint(cube_corners[a0], cube_corners[b0]);
                    Vec3f vert1 = midpoint(cube_corners[a1], cube_corners[b1]);
                    Vec3f vert2 = midpoint(cube_corners[a2], cube_corners[b2]);

                    Vec3f edge0  = vec3f_sub(vert1, vert0);
                    Vec3f edge1  = vec3f_sub(vert2, vert0);
                    Vec3f normal = vec3f_normalize(vec3f_cross(edge0, edge1));

                    array_append(&m.vertices, homogenize(vert0, res));
                    array_append(&m.vertices, homogenize(vert1, res));
                    array_append(&m.vertices, homogenize(vert2, res));

                    array_append(&m.normals, normal);
                    array_append(&m.normals, normal);
                    array_append(&m.normals, normal);

                    array_append(&m.uvs, init_vec2f(0, 0));
                    array_append(&m.uvs, init_vec2f(0, 1));
                    array_append(&m.uvs, init_vec2f(1, 0));
                }
            }
        }
    }
    return m;
}
