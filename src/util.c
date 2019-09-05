#include <stdlib.h>

#include "util.h"

char *load_file(const char *file_path)
{
    FILE *file = fopen(file_path, "rb");

    char *buffer = 0;
    if (file)
    {
        fseek(file, 0, SEEK_END);
        long length = ftell(file);
        fseek(file, 0, SEEK_SET);
        buffer = malloc(length+1);
        if (!buffer)
            return buffer;
        fread(buffer, 1, length, file);
        buffer[length] = 0;
        fclose(file);
    }

    return buffer;
}

void get_direction_angles(const Vec3f dir, float *h_angle, float *v_angle)
{
    Vec3f right = init_vec3f(1, 0, 0);
    
    Vec3f backward = init_vec3f(0, 0, 0);
    Vec3f forward = init_vec3f(0, 0, 1);
        
    Vec3f up = init_vec3f(0, 1, 0);

    Vec3f dir_hor = vec3f_normalize(
        init_vec3f(
            dir.x,
            0,
            dir.z
        ));
    if (vec3f_mag(dir_hor) == 0)
        *h_angle = 0;
    else
        *h_angle = atan2(vec3f_dot(up, vec3f_cross(forward, dir_hor)), vec3f_dot(forward, dir_hor));
    if (*h_angle < 0) *h_angle += 2*M_PI;


    float vert_z = -sqrt(dir.x*dir.x + dir.z*dir.z);
    Vec3f dir_vert = vec3f_normalize(
        init_vec3f(
            0,
            dir.y,
            vert_z
        ));

    if (vec3f_mag(dir_vert) == 0)
        *v_angle = 0;
    else
        *v_angle = atan2(vec3f_dot(right, vec3f_cross(backward, dir_vert)), vec3f_dot(backward, dir_vert));
}
