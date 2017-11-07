#version 330
in vec2 uv;
uniform float u_time;
uniform sampler2D texBuffer;

void main(void) {

    vec2 t_uv = uv*sin(u_time)*cos(u_time*0.2);
    t_uv.x += sin(u_time+uv.y);
    t_uv.y -= sin((u_time*0.4)+uv.x);
    vec4 smpl_base = texture(texBuffer,t_uv);
    gl_FragColor = smpl_base;
}
