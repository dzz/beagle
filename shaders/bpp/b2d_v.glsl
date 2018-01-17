#version 440

layout( location = 0) in vec2 input_position;
layout( location = 1) in vec2 uv_position;

out vec2 uv;

void main(void) {

    gl_Position.xy = input_position.xy*0.1;
    gl_Position.z = 0;
    gl_Position.w = 1;

    uv = uv_position;
}
