#version 440

uniform sampler2D sprite;
in vec2 uv;

void main(void) {
    gl_FragColor = texture(  sprite, uv );
}
