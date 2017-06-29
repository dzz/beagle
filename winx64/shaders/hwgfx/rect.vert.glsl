#version 330 core

in vec2 in_Position;
uniform vec2 scr_size;
uniform vec4 rect_coords;
out vec2 uv;

void main(void) {
    vec2 p_t;

    p_t.x = in_Position.x * rect_coords.z;
    p_t.y = in_Position.y * rect_coords.w;
    p_t.x += rect_coords.x;
    p_t.y += rect_coords.y;
    p_t.x /= scr_size.x;
    p_t.y = scr_size.y - p_t.y;
    p_t.y /= scr_size.y;
    p_t *= 2;
    p_t -= 1;

    uv.x = in_Position.x;
    uv.y = in_Position.y;
    gl_Position = vec4(p_t.x,p_t.y,0.0,1.0);
}
