#version 330

// @description: passes through texture pixels unchanged

uniform sampler2D texBuffer;
uniform float impulse;
in vec2 uv;

void main(void) {
    vec4 smpl_base= texture(texBuffer, uv);


    vec4 impulse_color = vec4(0.0,1.0,0.0,1.0);
    gl_FragColor = ((1.0-impulse)*smpl_base) + ((impulse_color * impulse)*smpl_base.a);
}

