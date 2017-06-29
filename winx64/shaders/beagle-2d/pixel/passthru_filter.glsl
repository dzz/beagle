#version 330

// @description: passes through texture pixels, tints by a filter_color

uniform sampler2D texBuffer;
uniform vec4 filter_color;
in vec2 uv;

void main(void) {
    vec4 smpl_base = texture(texBuffer,uv);
    gl_FragColor = smpl_base * filter_color;
}

