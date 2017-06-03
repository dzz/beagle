#version 330

in vec2 uv;
uniform float amt;

void main(void) {
    gl_FragColor = vec4(0.0,0.0,0.01,amt);
}
