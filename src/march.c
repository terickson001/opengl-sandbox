#include "march.h"

float sphere_dist(Vec3f p)
{
    return vec3f_mag(p)-1.0;
}

float scene_dist(Vec3f p)
{
    return sphere_dist(p);
}

#define MAX_STEPS (1024)
#define THRESHOLD 0.1f
float march(Vec3f eye, Vec3f ray, float max_distance)
{
    float depth = 0;
    for (int i = 0; i < MAX_STEPS; i++)
    {
        float dist = scene_dist(vec3f_add(eye, vec3f_scale(ray, depth)));

        if (dist < THRESHOLD)
            return depth;

        depth += dist;

        if (depth >= max_distance)
            return max_distance;
    }
    return max_distance;
}

