#version 330 core

in  vec2 in_Position;
out vec2 uv;

void main(void) {
        gl_Position = vec4( (in_Position.x*2)-1, (-1*(in_Position.y-0.5))*2, 
                0.0, 1.0);
        uv.x = in_Position.x;
        uv.y = 1 - in_Position.y;
}
