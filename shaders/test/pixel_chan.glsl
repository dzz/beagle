#version 440 

// @description: passes through texture pixels unchanged

uniform sampler2D texBuffer;

uniform float u_time;
in vec2 uv;
in vec4 fc;

void main(void) {
    vec2 t_uv = uv*sin(u_time)*cos(u_time*0.9);
    t_uv.x += sin((u_time*3)+uv.y);
    t_uv.y -= sin((u_time*0.4)+uv.x);
    vec4 smpl_base = texture(texBuffer,t_uv)*fc;
    gl_FragColor = smpl_base;
}

