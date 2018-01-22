#version 440

uniform sampler2D sprite;
in vec2 uv;
in vec4 fc;
in vec4 flash_col;

void main(void) {
    vec4 smpl_base = texture(sprite,uv)*fc;
    smpl_base.rgb = (smpl_base.rgb*(1.0-flash_col.a)) + (flash_col.rgb*flash_col.a);
    gl_FragColor = smpl_base;
}
