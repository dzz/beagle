#version 440

layout( location = 0) in vec2 input_position;
layout( location = 1) in float input_height;

out vec2 position;
out vec2 uv;

uniform vec2 camera_view;
uniform vec2 camera_position;
uniform float camera_zoom;

vec4 camera_transform( vec2 p, vec2 view, vec2 position, float zoom ) {
    vec4 transformed;
    transformed.xy = (p.xy - position) * view * zoom;
    transformed.z = 0.0;
    transformed.w = 1.0;
    return transformed;
}

void main(void) {
}

