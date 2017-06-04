#version 330 core

//VERTEX SHADER

uniform vec2 view;
uniform float scale;
uniform vec2 translation;

in vec2 input_position;
in vec2 uv_position;

out vec2 uv;

void main(void) {
    gl_Position.x =  ((input_position.x+translation.x)*scale) * view.x;
    gl_Position.y =  ((input_position.y+translation.y)*scale) * view.y;
    gl_Position.z = 0;
    gl_Position.w = 1;
    uv=uv_position;

}
