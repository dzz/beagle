#version 330

in vec2 uv;
uniform sampler2D light_texture;

void main(void) {
    vec4 smpl_base= texture(light_texture, uv);
    gl_FragColor = smpl_base;
}
