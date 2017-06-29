#version 330 core

uniform vec4 color;
in vec2 uv;

void main(void) {
    gl_FragColor = color;
}
