#version 330 core

// @description: transforms a unit square into a location within a view

uniform vec2 view;
uniform vec2 scale_local;
uniform vec2 scale_world;
uniform vec2 translation_local;
uniform vec2 translation_world;

in vec2 input_position;
in vec2 uv_position;

out vec2 uv;

void main(void) {
    
    float xt = (input_position.x+translation_local.x)*scale_local.x;
    float yt = (input_position.y+translation_local.y)*scale_local.y;

    gl_Position.x =  ((xt)+translation_world.x) * scale_world.x * view.x;
    gl_Position.y =  ((yt)+translation_world.y) * scale_world.y * view.y;
    gl_Position.z = 0;
    gl_Position.w = 1;

    uv=uv_position;
        
}

