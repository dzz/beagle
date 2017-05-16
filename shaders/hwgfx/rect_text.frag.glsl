#version 330 core

uniform sampler2D brush_texture;
uniform vec2 view;

in vec2 uv;
out vec4 color;

void main(void) {
    vec4 text_col = texture(brush_texture,uv);
    color.r = text_col.r;
    color.g = text_col.g;
    color.b = text_col.b;
    color.a = text_col.a;

}
