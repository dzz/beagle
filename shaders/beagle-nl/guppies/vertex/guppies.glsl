#version 440 

// @description: transforms a unit square into a location within a view

uniform vec2 view;
//uniform vec2 scale_local;
//uniform vec2 scale_world;
//uniform vec2 translation_local;
//uniform vec2 translation_world;
//uniform float rotation_local;

layout( location = 0) in vec2 input_position;
layout( location = 1) in vec2 uv_position;
layout( location = 2) in float rotation_local;
layout( location = 3) in vec2 scale_local;
layout( location = 4) in vec2 scale_world;
layout( location = 5) in vec2 translation_local;
layout( location = 6) in vec2 translation_world;
layout( location = 7) in vec4 filter_color;
//in vec2 scale_local;
//in vec2 scale_world;

out vec2 uv;
out vec4 fc;

void main(void) {
    

    //vec2 scale_local = vec2(1.0,1.0);
    //vec2 scale_world = vec2(1.0,1.0);
    //vec2 translation_local = vec2(1.0,1.0);
    //vec2 translation_world = vec2(1.0,1.0);
    //float rotation_local = 0.0;

    float x = (input_position.x+translation_local.x)*scale_local.x;
    float y = (input_position.y+translation_local.y)*scale_local.y;

    float xt = x * cos(rotation_local)  - y * sin(rotation_local);
    float yt = x * sin(rotation_local)  + y * cos(rotation_local);

    gl_Position.x =  ((xt)+translation_world.x) * scale_world.x * view.x;
    gl_Position.y =  ((yt)+translation_world.y) * scale_world.y * view.y;
    gl_Position.z = 0;
    gl_Position.w = 1;

    uv=uv_position;
    fc=filter_color;        
}

