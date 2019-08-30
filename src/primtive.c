#include "primitive.h"

#include "lib.h"
#include "glmath2.h"

#pragma GCC diagnostic ignored "-Wmissing-braces"

// BL BR TL TR
Vec3f cube_verts[24] = {
    {-1, -1,  1}, { 1, -1,  1}, {-1,  1,  1}, { 1,  1,  1}, // Front
    { 1, -1,  1}, { 1, -1, -1}, { 1,  1,  1}, { 1,  1, -1}, // Right
    { 1, -1, -1}, {-1, -1, -1}, { 1,  1, -1}, {-1,  1, -1}, // Back
    {-1, -1, -1}, {-1, -1,  1}, {-1,  1, -1}, {-1,  1,  1}, // Left
    {-1,  1,  1}, { 1,  1,  1}, {-1,  1, -1}, { 1,  1, -1}, // Top
    {-1, -1, -1}, { 1, -1, -1}, {-1, -1,  1}, { 1, -1,  1}, // Bottom
};
// ({2,0,0}*1/3 - {0,2,0}*0) * (1/(1/3*1/3 - 0*0))
Vec2f cube_uvs[24] = {
    {1/3.0f, 1/3.0f}, {2/3.0f, 1/3.0f}, {1/3.0f, 2/3.0f}, {2/3.0f, 2/3.0f}, // Front
    {2/3.0f, 1/3.0f}, {3/3.0f, 1/3.0f}, {2/3.0f, 2/3.0f}, {3/3.0f, 2/3.0f}, // Right
    {2/3.0f, 0/3.0f}, {3/3.0f, 0/3.0f}, {2/3.0f, 1/3.0f}, {3/3.0f, 1/3.0f}, // Back
    {0/3.0f, 1/3.0f}, {1/3.0f, 1/3.0f}, {0/3.0f, 2/3.0f}, {1/3.0f, 2/3.0f}, // Left
    {1/3.0f, 2/3.0f}, {2/3.0f, 2/3.0f}, {1/3.0f, 3/3.0f}, {2/3.0f, 3/3.0f}, // Top
    {1/3.0f, 0/3.0f}, {2/3.0f, 0/3.0f}, {1/3.0f, 1/3.0f}, {2/3.0f, 1/3.0f}, // Bottom
};

Vec3f cube_normals[24] = {
    { 0,  0,  1}, { 0,  0,  1}, { 0,  0,  1}, { 0,  0,  1}, // Front
    { 1,  0,  0}, { 1,  0,  0}, { 1,  0,  0}, { 1,  0,  0}, // Right
    { 0,  0, -1}, { 0,  0, -1}, { 0,  0, -1}, { 0,  0, -1}, // Back
    {-1,  0,  0}, {-1,  0,  0}, {-1,  0,  0}, {-1,  0,  0}, // Left
    { 0,  1,  0}, { 0,  1,  0}, { 0,  1,  0}, { 0,  1,  0}, // Top
    { 0, -1,  0}, { 0, -1,  0}, { 0, -1,  0}, { 0, -1,  0}, // Bottom
};

Vec3f cube_tangents[24] = {
    { 1,  0,  0}, { 1,  0,  0}, { 1,  0,  0}, { 1,  0,  0}, // Front
    { 0,  0, -1}, { 0,  0, -1}, { 0,  0, -1}, { 0,  0, -1}, // Right
    {-1,  0,  0}, {-1,  0,  0}, {-1,  0,  0}, {-1,  0,  0}, // Back
    { 0,  0,  1}, { 0,  0,  1}, { 0,  0,  1}, { 0,  0,  1}, // Left
    { 1,  0,  0}, { 1,  0,  0}, { 1,  0,  0}, { 1,  0,  0}, // Top
    {-1,  0,  0}, {-1,  0,  0}, {-1,  0,  0}, {-1,  0,  0}, // Bottom
};

Vec3f cube_bitangents[24] = {
    { 0,  1,  0}, { 0,  1,  0}, { 0,  1,  0}, { 0,  1,  0}, // Front
    { 0,  1,  0}, { 0,  1,  0}, { 0,  1,  0}, { 0,  1,  0}, // Right
    { 0,  1,  0}, { 0,  1,  0}, { 0,  1,  0}, { 0,  1,  0}, // Back
    { 0,  1,  0}, { 0,  1,  0}, { 0,  1,  0}, { 0,  1,  0}, // Left
    { 0,  0, -1}, { 0,  0, -1}, { 0,  0, -1}, { 0,  0, -1}, // Top
    { 0,  0,  1}, { 0,  0,  1}, { 0,  0,  1}, { 0,  0,  1}, // Front
};

u16 cube_indices[36] = {
     0, 1, 2,  1, 3, 2, // Front
     4, 5, 6,  5, 7, 6, // Right
     8, 9,10,  9,11,10, // Back
    12,13,14, 13,15,14, // Left
    16,17,18, 17,19,18, // Top
    20,21,22, 21,23,22, // Bottom
};

Model prim_cube = {0};
Model get_prim_cube()
{
    if (!prim_cube.vertices)
    {
        array_init(&prim_cube.vertices);
        array_init(&prim_cube.uvs);
        array_init(&prim_cube.normals);
        array_init(&prim_cube.indices);
        array_init(&prim_cube.tangents);
        array_init(&prim_cube.bitangents);

        array_appendv(&prim_cube.vertices,   cube_verts,      24);
        array_appendv(&prim_cube.uvs,        cube_uvs,        24);
        array_appendv(&prim_cube.normals,    cube_normals,    24);
        array_appendv(&prim_cube.tangents,   cube_tangents,   24);
        array_appendv(&prim_cube.bitangents, cube_bitangents, 24);
        array_appendv(&prim_cube.indices,    cube_indices,    36);

        // compute_tangent_basis(&prim_cube);
        
        prim_cube.indexed = true;
        
        create_model_vbos(&prim_cube);
    }
    return prim_cube;
}

#define H (0.73205080756f)
Vec3f pyramid_verts[16] = {
    {-1, -1, -1}, { 1, -1, -1}, {-1, -1,  1}, { 1, -1,  1}, // Base
    {-1, -1,  1}, { 1, -1,  1}, { 0,  H,  0},               // Front
    { 1, -1,  1}, { 1, -1, -1}, { 0,  H,  0},               // Right
    { 1, -1, -1}, {-1, -1, -1}, { 0,  H,  0},               // Back
    {-1, -1, -1}, {-1, -1,  1}, { 0,  H,  0},               // Left
};
#undef H

#define A (-RAD(30))
Vec3f pyramid_normals[16] = {
    {0, -1,  0}, { 0, -1,  0}, { 0, -1,  0}, { 0, -1,  0},                  // Base
    {0, -sin(A), cos(A)},   {0, -sin(A), cos(A)},   {0, -sin(A), cos(A)},   // Front
    {cos(A), -sin(A), 0},   {cos(A), -sin(A), 0},   {cos(A), -sin(A), 0},   // Right
    {0, sin(-A), -cos(-A)}, {0, sin(-A), -cos(-A)}, {0, sin(-A), -cos(-A)}, // Back
    {-cos(-A), sin(-A), 0}, {-cos(-A), sin(-A), 0}, {-cos(-A), sin(-A), 0}, // Left
};

Vec3f pyramid_tangents[16] = {
    {-1,  0,  0}, {-1,  0,  0}, {-1,  0,  0}, {-1,  0,  0}, // Base
    { 1,  0,  0}, { 1,  0,  0}, { 1,  0,  0},               // Front
    { 0,  0,  1}, { 0,  0,  1}, { 0,  0,  1},               // Right
    {-1,  0,  0}, {-1,  0,  0}, {-1,  0,  0},               // Back
    { 0,  0, -1}, { 0,  0, -1}, { 0,  0, -1},               // Left
};

Vec3f pyramid_bitangents[16] = {
    { 0,  0,  1}, { 0,  0,  1}, { 0,  0,  1}, { 0,  0,  1},                     // Base
    {0, cos(A), sin(A)},  {0, cos(A), sin(A)}, {0, cos(A), sin(A)},             // Front
    {-sin(A), cos(A), 0},  {-sin(A), cos(A), 0}, {-sin(A), cos(A), 0},          // Right
    {0, -cos(-A), -sin(-A)},  {0, -cos(-A), -sin(-A)}, {0, -cos(-A), -sin(-A)}, // Front
    {-sin(A), cos(A), 0},  {-sin(A), cos(A), 0}, {-sin(A), cos(A), 0},          // Left
};
#undef A

Vec2f pyramid_uvs[16] = {
    {1/3.0f, 0/3.0f}, {2/3.0f, 0/3.0f}, {1/3.0f, 1/3.0f}, {2/3.0f, 1/3.0f}, // Base
    {1/3.0f, 1/3.0f}, {2/3.0f, 1/3.0f}, {1/2.0f, 2/3.0f},                   // Front
    {1/3.0f, 1/3.0f}, {2/3.0f, 1/3.0f}, {1/2.0f, 2/3.0f},                   // Right
    {1/3.0f, 1/3.0f}, {2/3.0f, 1/3.0f}, {1/2.0f, 2/3.0f},                   // Back
    {1/3.0f, 1/3.0f}, {2/3.0f, 1/3.0f}, {1/2.0f, 2/3.0f},                   // Left
};

u16 pyramid_indices[18] = {
    0, 1, 2,  1, 3, 2, // Base
    4, 5, 6,           // Front
    7, 8, 9,           // Right
    10,11,12,          // Back
    13,14,15,          // Left
};

Model prim_pyramid = {0};
Model get_prim_pyramid()
{
    if (!prim_pyramid.vertices)
    {
        array_init(&prim_pyramid.vertices);
        array_init(&prim_pyramid.uvs);
        array_init(&prim_pyramid.normals);
        array_init(&prim_pyramid.indices);
        array_init(&prim_pyramid.tangents);
        array_init(&prim_pyramid.bitangents);

        array_appendv(&prim_pyramid.vertices,    pyramid_verts,      16);
        array_appendv(&prim_pyramid.uvs,         pyramid_uvs,        16);
        array_appendv(&prim_pyramid.normals,     pyramid_normals,    16);
        array_appendv(&prim_pyramid.tangents,    pyramid_tangents,   16);
        array_appendv(&prim_pyramid.bitangents,  pyramid_bitangents, 16);
        array_appendv(&prim_pyramid.indices,     pyramid_indices,    18);

        prim_pyramid.indexed = true;
        
        create_model_vbos(&prim_pyramid);
    }
    return prim_pyramid;
}

#define W (0.57735026918f)
Vec3f diamond_verts[24] = {
    {-W,  0,  W}, { W,  0,  W}, { 0,  1,  0},               // Top Front
    { W,  0,  W}, { W,  0, -W}, { 0,  1,  0},               // Top Right
    { W,  0, -W}, {-W,  0, -W}, { 0,  1,  0},               // Top Back
    {-W,  0, -W}, {-W,  0,  W}, { 0,  1,  0},               // Top Left
    
    { W,  0,  W}, {-W,  0,  W}, { 0, -1,  0},               // Bottom Front
    { W,  0, -W}, { W,  0,  W}, { 0, -1,  0},               // Bottom Right
    {-W,  0, -W}, { W,  0, -W}, { 0, -1,  0},               // Bottom Back
    {-W,  0,  W}, {-W,  0, -W}, { 0, -1,  0},               // Bottom Left
};
#undef W

#define A (-RAD(30))
Vec3f diamond_normals[24] = {
    {0, -sin(A), cos(A)}, {0, -sin(A), cos(A)}, {0, -sin(A), cos(A)}, // Top Front
    {cos(A), -sin(A), 0}, {cos(A), -sin(A), 0}, {cos(A), -sin(A), 0}, // Top Right
    {0, sin(-A), -cos(-A)}, {0, sin(-A), -cos(-A)}, {0, sin(-A), -cos(-A)}, // Top Back
    {-cos(-A), sin(-A), 0}, {-cos(-A), sin(-A), 0}, {-cos(-A), sin(-A), 0}, // Top Left
    
    {0, -sin(-A), cos(-A)}, {0, -sin(-A), cos(-A)}, {0, -sin(-A), cos(-A)}, // Bottom Front
    {cos(-A), -sin(-A), 0}, {cos(-A), -sin(-A), 0}, {cos(-A), -sin(-A), 0}, // Bottom Right
    {0, sin(A), -cos(A)}, {0, sin(A), -cos(A)}, {0, sin(A), -cos(A)}, // Bottom Back
    {-cos(A), sin(A), 0}, {-cos(A), sin(A), 0}, {-cos(A), sin(A), 0}, // Bottom Left
};
#undef A

Vec2f diamond_uvs[24] = {
    {1/3.0f, 1/3.0f}, {2/3.0f, 1/3.0f}, {1/2.0f, 2/3.0f},                   // Top Front
    {1/3.0f, 1/3.0f}, {2/3.0f, 1/3.0f}, {1/2.0f, 2/3.0f},                   // Top Right
    {1/3.0f, 1/3.0f}, {2/3.0f, 1/3.0f}, {1/2.0f, 2/3.0f},                   // Top Back
    {1/3.0f, 1/3.0f}, {2/3.0f, 1/3.0f}, {1/2.0f, 2/3.0f},                   // Top Left
    
    {1/3.0f, 1/3.0f}, {2/3.0f, 1/3.0f}, {1/2.0f, 2/3.0f},                   // Bottom Front
    {1/3.0f, 1/3.0f}, {2/3.0f, 1/3.0f}, {1/2.0f, 2/3.0f},                   // Bottom Right
    {1/3.0f, 1/3.0f}, {2/3.0f, 1/3.0f}, {1/2.0f, 2/3.0f},                   // Bottom Back
    {1/3.0f, 1/3.0f}, {2/3.0f, 1/3.0f}, {1/2.0f, 2/3.0f},                   // Bottom Left
};

u16 diamond_indices[24] = {
    0, 1, 2,  // Top Front
    3, 4, 5,  // Top Right
    6, 7, 8,  // Top Back
    9, 10,11, // Top Left
    
    12,13,14, // Bottom Front
    15,16,17, // Bottom Right
    18,19,20, // Bottom Back
    21,22,23, // Bottom Left
};

Model prim_diamond = {0};
Model get_prim_diamond()
{
    if (!prim_diamond.vertices)
    {
        array_init(&prim_diamond.vertices);
        array_init(&prim_diamond.uvs);
        array_init(&prim_diamond.normals);
        array_init(&prim_diamond.indices);

        array_appendv(&prim_diamond.vertices, diamond_verts,   24);
        array_appendv(&prim_diamond.uvs,      diamond_uvs,     24);
        array_appendv(&prim_diamond.normals,  diamond_normals, 24);
        array_appendv(&prim_diamond.indices,  diamond_indices, 24);

        compute_tangent_basis(&prim_diamond);
        prim_diamond.indexed = true;
        
        create_model_vbos(&prim_diamond);
    }
    return prim_diamond;
}
