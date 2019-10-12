#version 330 core

in vec3 uv;

uniform sampler2DArray texture_sampler;

out vec4 color;

float median(vec3 texel)
{
    return max(min(texel.r, texel.g), min(max(texel.r, texel.g), texel.b));
}

void main()
{
    vec2 pos = uv.xy;
    vec3 texel = texture(texture_sampler, uv).rgb;
    ivec2 sz = textureSize(texture_sampler, 0).xy;
    float dx = dFdx(pos.x) * sz.x;
    float dy = dFdx(pos.y) * sz.y;
    float to_pixels = 8.0 * inversesqrt(dx*dx + dy*dy);
    float sig_dist = median(texel);
    float w = fwidth(sig_dist);
    float opacity = smoothstep(0.5 -w, 0.5 + w, sig_dist);

    // color = vec4(1, 1, 1, opacity);
    color = vec4(1);
}
