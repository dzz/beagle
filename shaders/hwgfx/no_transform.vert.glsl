#version 330 core

// @description: passes through vertexes, nilling out z

in vec2 input_position;
in vec2 uv_position;

out vec2 uv;

void main(void) {
    gl_Position.x =  input_position.x;
    gl_Position.y =  input_position.y;
    gl_Position.z = 0;
    gl_Position.w = 1;
    uv=uv_position;

}

