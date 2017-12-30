#version 330

in vec2 uv;
uniform sampler2D precomputed_light_texture;
uniform vec3 color;

void main(void) {

    vec2 tuv = uv;
    tuv.y = 1.0-uv.y;
    vec4 smpl_base= texture(precomputed_light_texture, tuv);

    smpl_base.rgb *= color;
    gl_FragColor = smpl_base;
}
