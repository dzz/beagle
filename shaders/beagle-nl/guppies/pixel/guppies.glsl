#version 440 

// @description: passes through texture pixels unchanged

uniform sampler2D texBuffer;

in vec2 uv;
in vec4 fc;
in vec4 flash_col;

void main(void) {
    vec4 smpl_base = texture(texBuffer,uv)*fc;
    smpl_base.rgb = (smpl_base.rgb*(1.0-flash_col.a)) + (flash_col.rgb*flash_col.a);
    gl_FragColor = smpl_base;
}

