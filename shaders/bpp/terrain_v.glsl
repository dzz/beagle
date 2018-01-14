#version 440 

layout( location = 0) in vec2 input_position;
layout( location = 1) in float input_height;

out float height;

void main(void) {

    gl_Position.xy = (vec2(-1*input_position.x, -1*input_position.y)) * (1.0/32);
    gl_Position.z = 0;
    gl_Position.w = 1;

    height = input_height;
}
