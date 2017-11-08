#version 330 core

//VERTEX SHADER

in vec2 input_position;
in vec2 uv_position;

out vec2 uv;

void main(void) {
    gl_Position.xy =  input_position.xy;
    gl_Position.z = 0;
    gl_Position.w = 1; //important
    uv=uv_position;
}
