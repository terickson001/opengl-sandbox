#version 330 core

in vec3 position;

out VS_OUT {
    vec2 uv;
    vec3 position_m;
    vec3 normal_mv;
    vec3 eye_direction_mv;
    vec3 light_direction_mv;

    vec3 eye_direction_tbn;
    vec3 light_direction_tbn;
} vert;

void main()
{
    gl_Position = vec4(position, 0);
}
