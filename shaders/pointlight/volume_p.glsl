#version 330

in vec2 uv;

void main(void) {
    gl_FragColor.rgb = vec3(0.0,0.0,0.0);
    gl_FragColor.a = 1.0-(abs(uv.x*uv.x*uv.x));
}
