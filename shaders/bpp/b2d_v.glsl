#version 440

uniform vec2 camera_view;
uniform vec2 camera_position;
uniform float camera_zoom;

layout( location = 0) in vec2 input_position;
layout( location = 1) in vec2 uv_position;

layout( location = 2) in vec2 translation_local;
layout( location = 3) in vec2 scale_local;
layout( location = 4) in float rotation_local;
layout( location = 5) in vec2 translation_world;
layout( location = 6) in vec2 scale_world;
layout( location = 7) in vec4 filter_color;
layout( location = 8) in vec4 flash_color;

out vec2 uv;
out vec4 fc;
out vec4 flash_col;

vec4 camera_transform( vec2 p, vec2 view, vec2 position, float zoom ) {

    vec4 transformed;
    transformed.xy = (p.xy - position) * view * zoom;
    transformed.z = 0.0;
    transformed.w = 1.0;
    return transformed;
}

void main(void) {

    vec2 xy = (input_position.xy + translation_local) * scale_local;
    vec2 xyt;
    xyt = xy;
    xyt.x = xy.x * cos(rotation_local) - xy.y * sin(rotation_local);
    xyt.y = xy.x * sin(rotation_local) + xy.y * cos(rotation_local);


    xyt = (xyt+translation_world) * scale_world;

    vec4 xyc = camera_transform( xyt, camera_view, camera_position, camera_zoom );

    gl_Position = xyc;
   // gl_Position.xy = xyt;
    uv = uv_position;
    fc = filter_color;
    flash_col = flash_color;
}
