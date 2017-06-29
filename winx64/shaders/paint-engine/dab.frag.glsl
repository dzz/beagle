#version 330 core

uniform sampler2D dab_texture;
precision highp float;
uniform vec4 base_color;
uniform float noise;

in vec2 uv;
out vec4 color;

float rand(vec2 co){
  return (0.5-fract(cos(dot(co,vec2(4.898,7.23))) * 23421.631));
}

void main(void) {
    float d;
    vec4 dab_src = texture(dab_texture,uv);

    float n1 = rand(vec2( gl_FragCoord.x, gl_FragCoord.y)) * noise;
    
    color.r = base_color.r;
    color.g = base_color.g;
    color.b = base_color.b;
    color.a = (1 - dab_src.r)*(1-n1)*base_color.a;
    color.a = color.a;
}
