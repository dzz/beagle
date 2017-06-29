#version 330 core

uniform vec2 scr_size;
uniform vec2 char_pos;

in vec2 in_Position;
in vec2 uvin;
out vec2 uv;
out vec2 shade_inf;

void main(void) {
    vec2 p_t;
    int label_x;
    int label_y;

    p_t = in_Position;
    p_t.x += char_pos.x;
    p_t.y += char_pos.y;
    p_t.x /= scr_size.x;
    p_t.y = scr_size.y - p_t.y;
    p_t.y /= scr_size.y;
    p_t *= 2;
    p_t -= 1;

    shade_inf = p_t;
    gl_Position = vec4(p_t.x,p_t.y,0.0,1.0);
    uv = uvin;

}
