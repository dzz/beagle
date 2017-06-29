#version 330 core

uniform sampler2D ctxt_sampler;
out vec4 color;
in vec2 uv;
 
void main(void) {
    color = texture(ctxt_sampler,uv);
}

