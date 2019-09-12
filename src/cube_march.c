#include "cube_march.h"

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
                    Vec3f normal = vec3f_cross(edge0, edge1);

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
