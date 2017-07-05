#version 330 core

uniform sampler2D font_texture;
uniform vec3 label_col;

in vec2 shade_inf;
in vec2 uv;
out vec4 color;

void main(void) {
    vec4 text_col = texture(font_texture,uv);
    color.r = label_col.r;
    color.g = label_col.g;
    color.b = label_col.b;
    color.a = text_col.r;
}
