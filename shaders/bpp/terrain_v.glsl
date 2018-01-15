#version 440 

layout( location = 0) in vec2 input_position;
layout( location = 1) in float input_height;

out float height;
out vec2 position;

/*****/
uniform vec2 camera_view;
uniform vec2 camera_position;
uniform float camera_zoom;
/*****/

vec4 camera_transform( vec2 p, vec2 view, vec2 position, float zoom ) {

    vec4 transformed;
    
    transformed.xy = (p.xy - position) * view * zoom;
    //transformed.xy = (transformed.xy+(-1*position)*zoom) * view;
    //transformed.xy = (transformed.xy);
    transformed.z = 0.0;
    transformed.w = 1.0;

    return transformed;
}

void main(void) {

    gl_Position = camera_transform( input_position, camera_view, camera_position, camera_zoom );

    height = input_height;
    position = input_position;
}
