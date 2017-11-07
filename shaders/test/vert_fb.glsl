#version 330 core

//VERTEX SHADER

in vec2 input_position;
in vec2 uv_position;
uniform sampler2D texBuffer;

out vec2 uv;

void main(void) {
    gl_Position.x =  input_position.x*0.6;
    gl_Position.y =  -1 *input_position.y*0.6;
    gl_Position.z = 0;
    gl_Position.w = 1; //important
    uv=uv_position;
}
