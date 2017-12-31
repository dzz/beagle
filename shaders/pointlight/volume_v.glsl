#version 440 

layout( location = 0) in vec2 input_position;
layout( location = 1) in vec2 uv_position;

out vec2 uv;

void main(void) {
    gl_Position.xy = vec2(input_position.x, -1*input_position.y);
    gl_Position.z = 0;
    gl_Position.w = 1;

    uv = uv_position;
}
