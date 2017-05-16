#version 330 core

in vec2 uv;

void main(void) {
    gl_FragColor = ((color0) * uv.y) + ((color1) * (1-uv.y));
}
