#version 330 core

in VS_OUT {
    vec2 uv;
    vec3 position_m;
    vec3 normal_mv;
    vec3 eye_direction_mv;
    vec3 light_direction_mv;

    vec3 eye_direction_tbn;
    vec3 light_direction_tbn;
} frag;

out vec3 color;

uniform sampler2D diffuse_sampler;
uniform sampler2D normal_sampler;
uniform sampler2D specular_sampler;

uniform vec3 light_color;
uniform float light_power;
uniform vec3 light_position_m;

const float specularity = 1;

float scene_dist(vec3 p)
{
    return length(p)-1.0;
}

float march(vec3 eye, vec3 ray, float max_distance)
{
    float depth = 0;
    for (int i = 0; i < 1024; i++)
    {
        float dist = scene_dist(eye + ray*depth);
        if (dist < 0.0001f)
            return depth;

        depth = depth + dist;

        if (depth >= max_distance)
            return max_distance;
    }
    return max_distance;
}

vec3 ray_direction(float fov, vec2 size, vec2 fragCoord)
{
    vec2 xy = fragCoord - size / 2.0;
    float z = size.y / tan(radians(fov))/2.0;
    return normalize(vec3(xy, -z));
}

void main()
{
    color = vec3(1, 0, 0);
    return;

    vec3 ray = ray_direction(45, vec2(1024, 768), gl_FragCoord.xy);
    vec3 eye = vec3(0, 0, 5);

    float max_dist = 100.0;
    float dist = march(eye, ray, max_dist);

    if (dist > max_dist - 0.0001)
    {
        color = vec3(1,0, 0);
        return;
    }
    
    color = vec3(1, 0, 0);
}
