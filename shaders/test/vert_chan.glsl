#version 440 

layout( location = 0) in vec2 input_position;
layout( location = 1) in vec2 uv_position;
layout( location = 2) in vec4 filter_color;

uniform float u_time;

out vec2 uv;
out vec4 fc;

void main(void) {

    gl_Position.xy = vec2(-1*input_position.x, -1*input_position.y) + (vec2(sin(u_time) + cos(u_time))*0.1);
    gl_Position.z = 0;
    gl_Position.w = 1;

    uv = uv_position;
    fc = filter_color;
}
