#version 430 core

in VS_OUT {
    vec2 uv;
    vec3 position_m;
    vec3 normal;
    vec3 normal_mv;
    vec3 eye_direction_mv;
    vec3 light_direction_mv;

    vec3 eye_direction_tbn;
    vec3 light_direction_tbn;
    vec3 tangent;
    vec3 bitangent;
    vec3 position;
} frag;

out vec3 color;

uniform sampler2D diffuse_sampler;
uniform sampler2D normal_sampler;
uniform sampler2D specular_sampler;

uniform vec3 light_color;
uniform float light_power;
uniform vec3 light_position_m;

const float specularity = 1;
void main()
{
    // vec3 xc = vec3(0.7, 0.3, 0.0);
    // vec3 yc = vec3(0.0, 0.7, 0.3);
    // vec3 zc = vec3(0.3, 0.0, 0.7);
    vec3 xc = vec3(1.0, 0.0, 0.0);
    vec3 yc = vec3(0.0, 1.0, 0.0);
    vec3 zc = vec3(0.0, 0.0, 1.0);

    vec3 pos_position = (frag.position+1)/2;
    vec3 material_diffuse_color = pos_position;
    //vec3 material_diffuse_color = pos_position.x*xc + pos_position.y+yc + pos_position.z+zc;
    //vec3 material_diffuse_color = (gl_FragCoord.x)/2048*xc+gl_FragCoord.y/1536*yc;//(frag.normal/2)+0.5;
    // vec3 material_diffuse_color = texture(diffuse_sampler, frag.uv).rgb;
    vec3 material_ambient_color = 0.3f * material_diffuse_color;
    vec3 material_specular_color = texture(specular_sampler, frag.uv).rgb * specularity;

    float dist = length(light_position_m - frag.position_m);

    vec3 n = normalize(texture(normal_sampler, vec2(frag.uv.x, frag.uv.y)).rgb*2.0 - 1.0);
    vec3 l = normalize(frag.light_direction_tbn);
    vec3 E = normalize(frag.eye_direction_tbn);
    
    vec3 R = reflect(-l, n);
    float cos_theta = clamp(dot(n, l), 0, 1);
    float cos_alpha = clamp(dot(E, R), 0, 1);

    color =
        // Ambient : simulates indirect lighting
        material_ambient_color +
        // Diffuse : "color" of the object
        material_diffuse_color * light_color * light_power * cos_theta / (dist*dist) +
        // Specular : reflective highlight, like a mirror
        material_specular_color * light_color * light_power * pow(cos_alpha, 5) / (dist*dist);
}
