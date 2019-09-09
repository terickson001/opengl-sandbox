#version 430 core

out vec4 color;

uniform vec3 cam_pos;
uniform mat4 V;
uniform mat4 P;

// uniform vec3 light_position;
// uniform vec3 light_color;
// uniform float light_power;

const float EPSILON = 0.0001;

float sphere_dist(vec3 p, float d)
{
    return length(p) - d;
}

float box_dist(vec3 p, float b)
{
    vec3 d = abs(p) - b;
    return length(max(d, 0.0))
        + min(max(d.x, max(d.y, d.z)), 0.0); 
}

float scene_dist(in vec3 p)
{
    return max(box_dist(p, 0.6), sphere_dist(p, 0.75));
}

float scene_dist(in vec3 p, out vec3 color)
{
    color = vec3(1, 0, 0);
    return max(box_dist(p, 0.8), -sphere_dist(p, 0.95));
}

float march(in vec3 eye, in vec3 ray, in float max_distance, out vec3 color)
{
    float depth = 0;
    for (int i = 0; i < 1024; i++)
    {
        float dist = scene_dist(eye + ray*depth, color);
        if (dist < EPSILON)
            return depth;

        depth = depth + dist;

        if (depth >= max_distance)
            return max_distance;
    }
    return max_distance;
}

vec3 estimate_normal(vec3 p)
{
    return normalize(vec3(
                         scene_dist(vec3(p.x+EPSILON, p.y, p.z)) - scene_dist(vec3(p.x-EPSILON, p.y, p.z)),
                         scene_dist(vec3(p.x, p.y+EPSILON, p.z)) - scene_dist(vec3(p.x, p.y-EPSILON, p.z)),
                         scene_dist(vec3(p.x, p.y, p.z+EPSILON)) - scene_dist(vec3(p.x, p.y, p.z-EPSILON))
                     ));
                  
}

vec3 ray_direction(float fov, vec2 size, vec2 fragCoord)
{
    vec2 xy = fragCoord - size / 2.0;
    float z = size.y / tan(radians(fov)/2.0);
    return normalize(vec3(xy, -z));
}

void main()
{
    vec3 light_position = vec3(4, 4, 4);
    vec3 light_color = vec3(1, 1, 1);
    float light_power = 20;
    
    vec3 ray = (V * vec4(ray_direction(45, vec2(1024, 768), gl_FragCoord.xy), 0)).xyz;
    // vec3 eye = vec3(0, 0, 5);
    
    float max_dist = 100.0;
    vec3 base_color;
    float dist = march(cam_pos, ray, max_dist, base_color);

    if (dist > max_dist - 0.0001)
    {
        // Didn't hit anything
        color = vec4(0, 0, 0, 0);
        return;
    }

    vec3 p = cam_pos + dist*ray;
    vec3 n = estimate_normal(p);
    float l_dist = length(light_position - p);
    
    vec3 l = normalize(light_position - p);
    vec3 E = normalize(cam_pos - p);
    vec3 R = normalize(reflect(-l, n));
    
    float cos_theta = clamp(dot(n, l), 0, 1);
    float cos_alpha = clamp(dot(E, R), 0, 1);

    // vec3 base_color = vec3(1, 0, 0);
    
    float ambience = 0.1;
    color = vec4(
        base_color * ambience +
        base_color * light_color * light_power * cos_theta / (l_dist*l_dist) +
        light_color * light_power * pow(cos_alpha, 5) / (l_dist*l_dist),
        1);
}
