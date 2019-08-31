#include "model.h"

#include <stdio.h>

#include "lib.h"

Model load_obj(const char *filepath)
{
    Model m = {0};
    FILE *file = fopen(filepath, "r");
    if (!file)
    {
        printf("Could not open file \"%s\"\n", filepath);
        return m;
    }

    uint *vertex_indices, *uv_indices, *normal_indices;
    Vec3f *temp_vertices, *temp_normals;
    Vec2f *temp_uvs;

    array_init(&vertex_indices);
    array_init(&uv_indices);
    array_init(&normal_indices);

    array_init(&temp_vertices);
    array_init(&temp_normals);
    array_init(&temp_uvs);
    
    for (;;)
    {
        char line_header[128];
        if (fscanf(file, "%s", line_header) == EOF)
            break;

        if (strcmp(line_header, "v") == 0)
        {
            Vec3f vertex = make_vec3f();
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            array_append(&temp_vertices, vertex);
        }
        else if (strcmp(line_header, "vn") == 0)
        {
            Vec3f normal = make_vec3f();
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            array_append(&temp_normals, normal);
        }
        else if (strcmp(line_header, "vt") == 0)
        {
            Vec2f uv = make_vec2f();
            fscanf(file, "%f %f\n", &uv.u, &uv.v);
            array_append(&temp_uvs, uv);
        }
        else if (strcmp(line_header, "f") == 0)
        {
            uint v_index[3], uv_index[3], n_index[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
                                 &v_index[0], &uv_index[0], &n_index[0],
                                 &v_index[1], &uv_index[1], &n_index[1],
                                 &v_index[2], &uv_index[2], &n_index[2]
            );
            if (matches != 9)
            {
                fprintf(stderr, "OBJ file \"%s\" cannot be parsed correctly. Try exporting with different options.\n", filepath);
                
                array_free(temp_vertices);
                array_free(temp_uvs);
                array_free(temp_normals);

                array_free(vertex_indices);
                array_free(uv_indices);
                array_free(normal_indices);

                return m;
            }
            array_append(&vertex_indices, v_index[0]);
            array_append(&vertex_indices, v_index[1]);
            array_append(&vertex_indices, v_index[2]);
            array_append(&uv_indices,     uv_index[0]);
            array_append(&uv_indices,     uv_index[1]);
            array_append(&uv_indices,     uv_index[2]);
            array_append(&normal_indices, n_index[0]);
            array_append(&normal_indices, n_index[1]);
            array_append(&normal_indices, n_index[2]);
        }
    }

    array_init(&m.vertices);
    array_init(&m.uvs);
    array_init(&m.normals);

    for (int i = 0; i < array_size(vertex_indices); i++)
    {
        uint v_index = vertex_indices[i];
        Vec3f vertex = temp_vertices[v_index-1];
        array_append(&m.vertices, vertex);
    }
    for (int i = 0; i < array_size(uv_indices); i++)
    {
        uint uv_index = uv_indices[i];
        Vec2f uv = temp_uvs[uv_index-1];
        array_append(&m.uvs, uv);
    }
    for (int i = 0; i < array_size(normal_indices); i++)
    {
        uint n_index = normal_indices[i];
        Vec3f normal = temp_normals[n_index-1];
        array_append(&m.normals, normal);
    }

    array_free(temp_vertices);
    array_free(temp_uvs);
    array_free(temp_normals);

    array_free(vertex_indices);
    array_free(uv_indices);
    array_free(normal_indices);

    array_shrink(&m.vertices);
    array_shrink(&m.uvs);
    array_shrink(&m.normals);

    return m;
}

typedef struct Packed_Vertex
{
    Vec3f v;
    Vec2f uv;
    Vec3f n;
} Packed_Vertex;

// Returns true iif v1 can be considered equal to v2
bool is_near(float v1, float v2){
    return fabsf( v1-v2 ) < 0.01f;
}

// Searches through all already-exported vertices
// for a similar one.
// Similar = same position + same UVs + same normal
bool _get_similar_vertex_slow( 
    Vec3f vertex, 
    Vec2f uv, 
    Vec3f normal,
    Vec3f *out_vertices,
    Vec2f *out_uvs,
    Vec3f *out_normals,
    u16 *result
){
    // Lame linear search
    for (u16 i=0; i < array_size(out_vertices); i++){
        if (
            is_near(vertex.x, out_vertices[i].x) &&
            is_near(vertex.y, out_vertices[i].y) &&
            is_near(vertex.z, out_vertices[i].z) &&
            is_near(uv.u,     out_uvs     [i].u) &&
            is_near(uv.v,     out_uvs     [i].v) &&
            is_near(normal.x, out_normals [i].x) &&
            is_near(normal.y, out_normals [i].y) &&
            is_near(normal.z, out_normals [i].z)
        ){
            *result = i;
            return true;
        }
    }
    // No other vertex could be used instead.
    // Looks like we'll have to add it to the VBO.
    return false;
}

HASHMAP(Vert, vert, u16);

bool _get_similar_vertex(Packed_Vertex vert, Vert_Map *m, u16 *out_index)
{
    u16 *res = vert_map_get(m, hash_crc64(&vert, sizeof(Packed_Vertex)));
    if (!res)
        return false;
    *out_index = *res;
    return true;
}

void index_model(Model *m)
{
    Vec3f *temp_vertices, *temp_normals;
    Vec2f *temp_uvs;
    Vec3f *temp_tangents, *temp_bitangents;

    array_init(&temp_vertices);
    array_init(&temp_normals);
    array_init(&temp_uvs);
    
    temp_tangents = 0;
    if (m->tangents)
        array_init(&temp_tangents);
    
    temp_bitangents = 0;
    if (m->bitangents)
        array_init(&temp_bitangents);

    array_init(&m->indices);
    
    for (u16 i = 0; i < (u16)array_size(m->vertices); i++)
    {
        u16 index;
        bool found = _get_similar_vertex_slow(m->vertices[i], m->uvs[i], m->normals[i],
                                              temp_vertices, temp_uvs, temp_normals,
                                              &index);
        
        if (found)
        {
            array_append(&m->indices, index);

            if (m->tangents)   temp_tangents[index]   = vec3f_add(temp_tangents[index],   m->tangents[i]);
            if (m->bitangents) temp_bitangents[index] = vec3f_add(temp_bitangents[index], m->bitangents[i]);
        }
        else
        {
            array_append(&temp_vertices,   m->vertices[i]);
            array_append(&temp_uvs,        m->uvs[i]);
            array_append(&temp_normals,    m->normals[i]);
            if (m->tangents)   array_append(&temp_tangents,   m->tangents[i]);
            if (m->bitangents) array_append(&temp_bitangents, m->bitangents[i]);
            
            u16 new_index = (u16)array_size(temp_vertices)-1;
            array_append(&m->indices, new_index);
        }
    }

    array_free(m->vertices);
    array_free(m->uvs);
    array_free(m->normals);
    array_free(m->tangents);
    array_free(m->bitangents);

    m->vertices   = temp_vertices;
    m->uvs        = temp_uvs;
    m->normals    = temp_normals;
    m->tangents   = temp_tangents;
    m->bitangents = temp_bitangents;

    array_shrink(&m->vertices);
    array_shrink(&m->uvs);
    array_shrink(&m->normals);
    if (m->tangents)   array_shrink(&m->tangents);
    if (m->bitangents) array_shrink(&m->bitangents);
    array_shrink(&m->indices);

    m->indexed = true;
}

void _compute_tangent_basis_indexed(Model *m)
{
    make_array_reserve(&m->tangents,   array_size(m->vertices));
    array_set_size(&m->tangents, array_capacity(m->tangents));
    make_array_reserve(&m->bitangents, array_size(m->vertices));
    array_set_size(&m->bitangents, array_capacity(m->bitangents));
    
    for (int i = 0; i < array_size(m->indices); i+=3)
    {
        u16 i1 = m->indices[i];
        u16 i2 = m->indices[i+1];
        u16 i3 = m->indices[i+2];
        
        Vec3f *v0 = &m->vertices[i1];
        Vec3f *v1 = &m->vertices[i2];
        Vec3f *v2 = &m->vertices[i3];

        Vec2f *uv0 = &m->uvs[i1];
        Vec2f *uv1 = &m->uvs[i2];
        Vec2f *uv2 = &m->uvs[i3];

        Vec3f delta_pos0 = vec3f_sub(*v1, *v0);
        Vec3f delta_pos1 = vec3f_sub(*v2, *v0);

        Vec2f delta_uv0 = vec2f_sub(*uv1, *uv0);
        Vec2f delta_uv1 = vec2f_sub(*uv2, *uv0);

        float r = 1.0f / (delta_uv0.u*delta_uv1.v - delta_uv0.v*delta_uv1.u);
        
        // tangent = (delta_pos0*delta_uv1.v - delta_pos1*delta_uv0.v)*r
        Vec3f tangent = vec3f_scale(
            vec3f_sub(
                vec3f_scale(delta_pos0, delta_uv1.v),
                vec3f_scale(delta_pos1, delta_uv0.v)
            ),
            r
        );
        
        // bitangent = (delta_pos1*delta_uv0.u - delta_pos0*delta_uv1.u)*r
        Vec3f bitangent = vec3f_scale(
            vec3f_sub(
                vec3f_scale(delta_pos1, delta_uv0.u),
                vec3f_scale(delta_pos0, delta_uv1.u)
            ),
            r
        );

        m->tangents[i1] = tangent;
        m->tangents[i2] = tangent;
        m->tangents[i3] = tangent;

        m->bitangents[i1] = bitangent;
        m->bitangents[i2] = bitangent;
        m->bitangents[i3] = bitangent;
    }
}

void _compute_tangent_basis_unindexed(Model *m)
{
    array_init(&m->tangents);
    array_init(&m->bitangents);
    
    for (int i = 0; i < array_size(m->vertices); i+=3)
    {
        Vec3f *v0 = &m->vertices[i];
        Vec3f *v1 = &m->vertices[i+1];
        Vec3f *v2 = &m->vertices[i+2];

        Vec2f *uv0 = &m->uvs[i];
        Vec2f *uv1 = &m->uvs[i+1];
        Vec2f *uv2 = &m->uvs[i+2];

        Vec3f delta_pos0 = vec3f_sub(*v1, *v0);
        Vec3f delta_pos1 = vec3f_sub(*v2, *v0);

        Vec2f delta_uv0 = vec2f_sub(*uv1, *uv0);
        Vec2f delta_uv1 = vec2f_sub(*uv2, *uv0);

        float r = 1.0f / (delta_uv0.u*delta_uv1.v - delta_uv0.v*delta_uv1.u);
        
        // tangent = (delta_pos0*delta_uv1.v - delta_pos1*delta_uv0.v)*r
        Vec3f tangent = vec3f_scale(
            vec3f_sub(
                vec3f_scale(delta_pos0, delta_uv1.v),
                vec3f_scale(delta_pos1, delta_uv0.v)
            ),
            r
        );
        
        // bitangent = (delta_pos1*delta_uv0.u - delta_pos0*delta_uv1.u)*r
        Vec3f bitangent = vec3f_scale(
            vec3f_sub(
                vec3f_scale(delta_pos1, delta_uv0.u),
                vec3f_scale(delta_pos0, delta_uv1.u)
            ),
            r
        );

        array_append(&m->tangents, tangent);
        array_append(&m->tangents, tangent);
        array_append(&m->tangents, tangent);

        array_append(&m->bitangents, bitangent);
        array_append(&m->bitangents, bitangent);
        array_append(&m->bitangents, bitangent);
    }
}

void compute_tangent_basis(Model *m)
{
    if (m->indexed)
        _compute_tangent_basis_indexed(m);
    else
        _compute_tangent_basis_unindexed(m);

    for (int i = 0; i < array_size(m->vertices); i++)
    {
        Vec3f *t = &m->tangents[i];
        Vec3f *b = &m->bitangents[i];
        Vec3f *n = &m->normals[i];

        // t = normalize(t - n * dot(n, t));
        *t = vec3f_normalize(vec3f_sub(*t, vec3f_scale(*n, vec3f_dot(*n, *t))));

        if (vec3f_dot(vec3f_cross(*n, *t), *b) < 0.0f)
            *t = vec3f_scale(*t, -1);
    }
    
    //array_shrink(&m->tangents);
    //array_shrink(&m->bitangents);
}

void create_model_vbos(Model *m)
{      
    glGenBuffers(1, &m->vbuff);
    glBindBuffer(GL_ARRAY_BUFFER, m->vbuff);
    glBufferData(GL_ARRAY_BUFFER, array_size(m->vertices)*sizeof(Vec3f), m->vertices, GL_STATIC_DRAW);

    if (m->uvs)
    {
        glGenBuffers(1, &m->uvbuff);
        glBindBuffer(GL_ARRAY_BUFFER, m->uvbuff);
        glBufferData(GL_ARRAY_BUFFER, array_size(m->uvs)*sizeof(Vec2f), m->uvs, GL_STATIC_DRAW);
    }

    if (m->normals)
    {
        glGenBuffers(1, &m->nbuff);
        glBindBuffer(GL_ARRAY_BUFFER, m->nbuff);
        glBufferData(GL_ARRAY_BUFFER, array_size(m->normals)*sizeof(Vec3f), m->normals, GL_STATIC_DRAW);
    }

    if (m->tangents)
    {
        glGenBuffers(1, &m->tbuff);
        glBindBuffer(GL_ARRAY_BUFFER, m->tbuff);
        glBufferData(GL_ARRAY_BUFFER, array_size(m->tangents)*sizeof(Vec3f), m->tangents, GL_STATIC_DRAW);
    }

    if (m->bitangents)
    {
        glGenBuffers(1, &m->btbuff);
        glBindBuffer(GL_ARRAY_BUFFER, m->btbuff);
        glBufferData(GL_ARRAY_BUFFER, array_size(m->bitangents)*sizeof(Vec3f), m->bitangents, GL_STATIC_DRAW);
    }

    if (m->indices)
    {
        glGenBuffers(1, &m->ebuff);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->ebuff);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, array_size(m->indices)*sizeof(u16), m->indices, GL_STATIC_DRAW);
    }
}

void invert_uvs(Model *m)
{
    for (int i = 0; i < array_size(m->uvs); i++)
        m->uvs[i].v = 1.0f - m->uvs[i].v;
}

Model make_model(char const *filepath, bool normals, bool invert_uv)
{
    Model model = load_obj(filepath);
    if (normals) compute_tangent_basis(&model);                          
    index_model(&model);
    if (invert_uv) invert_uvs(&model);

    return model;
}

void draw_model(Shader s, Model m)
{
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m.vbuff);
    glVertexAttribPointer(
        0,        // attribute 0
        3,        // size
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        0,        // stride
        0         // array buffer offset
    );

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, m.uvbuff);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, m.nbuff);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    if (m.tbuff)
    {
        glEnableVertexAttribArray(3);
        glBindBuffer(GL_ARRAY_BUFFER, m.tbuff);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }

    if (m.btbuff)
    {
        glEnableVertexAttribArray(4);
        glBindBuffer(GL_ARRAY_BUFFER, m.btbuff);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }

    if (m.indexed)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.ebuff);
        glDrawElements(GL_TRIANGLES, array_size(m.indices), GL_UNSIGNED_SHORT, 0);
    }
    else
    {
        glDrawArrays(GL_TRIANGLES, 0, array_size(m.vertices));
    }

    glDisableVertexAttribArray(0); // vbuff
    glDisableVertexAttribArray(1); // uvbuff
    glDisableVertexAttribArray(2); // nbuff
    if (m.tbuff)  glDisableVertexAttribArray(3); // tbuff
    if (m.btbuff) glDisableVertexAttribArray(4); // btbuff
}

void free_model(Model *m)
{
    array_free(m->vertices);
    array_free(m->uvs);
    array_free(m->normals);
    array_free(m->indices);
    array_free(m->tangents);
    array_free(m->bitangents);

    m->vertices   = 0;
    m->uvs        = 0;
    m->normals    = 0;
    m->indices    = 0;
    m->tangents   = 0;
    m->bitangents = 0;
}

void destroy_model(Model *m)
{
    free_model(m);
}
