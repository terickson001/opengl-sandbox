#version 430 core

out vec4 color;

uniform vec3 cam_pos;
uniform mat4 V;
uniform mat4 P;

uniform vec2 resolution;
uniform float global_time;

// uniform vec3 light_position;
// uniform vec3 light_color;
// uniform float light_power;

const float EPSILON = 0.0001;

// SDFs
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

float torus_dist(vec3 p, float g, float t)
{
    vec2 q = vec2(length(p.xz) - g, p.y);
    return length(q) - t;
}

float inf_cylindery_dist(vec3 p, float r)
{
    return length(p.xz) - r;
}

float inf_cylinderx_dist(vec3 p, float r)
{
    return length(p.yz) - r;
}

float inf_cylinderz_dist(vec3 p, float r)
{
    return length (p.xy) - r;
}

// Operations
float intersect_op(float a, float b)
{
    return max(a, b);
}

float union_op(float a, float b)
{
    return min(a, b);
}

float difference_op(float a, float b)
{
    return max(a, -b);
}

// Transformation
mat4 rotate_mat(float angle, vec3 axis)
{
    mat4 mat;

    float cos_a = cos(angle);
    float sin_a = sin(angle);

    vec3 normal = normalize(axis);
    vec3 temp = normal * (1.0-cos_a);

    mat[0][0] = temp.x*axis.x+cos_a;
    mat[0][1] = temp.x*axis.y-axis.z*sin_a;
    mat[0][2] = temp.x*axis.z+axis.y*sin_a;

    mat[1][0] = temp.y*axis.x+axis.z*sin_a;
    mat[1][1] = temp.y*axis.y+cos_a;
    mat[1][2] = temp.y*axis.z-axis.x*sin_a;

    mat[2][0] = temp.z*axis.x-axis.y*sin_a;
    mat[2][1] = temp.z*axis.y+axis.x*sin_a;
    mat[2][2] = temp.z*axis.z+cos_a;

    mat[3][3] = 1;

    return mat;
}

float scene_dist(in vec3 p)
{
    mat4 rot = rotate_mat(radians(45), vec3(0,0,1));
    
    float torus = torus_dist(p+vec3(0, sin(global_time), 0), 1, 0.5);
    float box = box_dist((rot*vec4(p,0)).xyz, 0.9);
    float sphere = sphere_dist(p, 1.2);
    float cylindery = inf_cylindery_dist(p, 0.4);
    float cylinderz = inf_cylinderz_dist(p, 0.4);
    float cylinderx = inf_cylinderx_dist(p, 0.4);

    return difference_op(difference_op(difference_op(difference_op(sphere, difference_op(torus, box)), cylindery), cylinderz), cylinderx);
}

float scene_dist(in vec3 p, out vec3 color)
{
    color = vec3(.7, .2, .2);
    return scene_dist(p);
    // return max(box_dist(p, 0.8), -sphere_dist(p, 0.95));
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
    float light_power = 15;
    
    vec3 view_ray = ray_direction(90, resolution, gl_FragCoord.xy);
    vec3 world_ray = (V * vec4(view_ray, 0)).xyz;
    // vec3 eye = vec3(0, 0, 5);
    
    float max_dist = 100.0;
    vec3 base_color;
    float dist = march(cam_pos, world_ray, max_dist, base_color);

    if (dist > max_dist - 0.0001)
    {
        // Didn't hit anything
        color = vec4(0, 0, 0, 0);
        return;
    }

    vec3 p = cam_pos + dist*world_ray;
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
