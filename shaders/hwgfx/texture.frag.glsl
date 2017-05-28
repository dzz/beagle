#version 330 core

uniform sampler2D ctxt_sampler;
in vec2 uv;
 
void main(void) {
    vec4 texel = texture(ctxt_sampler,uv);
    gl_FragColor = texel;
}
