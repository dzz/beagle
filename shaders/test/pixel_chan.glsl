#version 440 

// @description: passes through texture pixels unchanged

uniform float u_time;
in vec2 uv;
in vec4 fc;

void main(void) {
    vec4 smpl_base = fc;
    gl_FragColor = smpl_base;
}

