#version 330
in vec2 uv;
uniform float u_time;

void main(void) {

    vec2 t_uv = uv*(sin(u_time)*cos(u_time*2))*2;
    gl_FragColor = vec4( sin(t_uv.x+u_time),cos(t_uv.y-u_time),t_uv.x*t_uv.y,1.0);
}
