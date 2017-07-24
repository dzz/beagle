#version 440 

// @description: passes through texture pixels unchanged

uniform sampler2D texBuffer;

in vec2 uv;
in vec4 fc;

void main(void) {
    vec4 smpl_base = texture(texBuffer,uv)*fc;
    gl_FragColor = smpl_base;
}

