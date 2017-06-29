#version 330 core

precision highp float;
// x,y  = canvas space
//   z  = dab size
uniform vec3 dab_location;

// width/height of canvas 
uniform vec2 scr_size;
uniform float rot;

// this will be a unit -1,1 square
in  vec2 in_Position;
out vec2 uv;

void main(void) {
    vec2 p_t;
    mat2 rot_mat = mat2( cos( rot ), -sin( rot ),
                         sin( rot ),  cos( rot ) );

    p_t = in_Position * rot_mat;
    p_t.x = p_t.x * dab_location.z;
    p_t.y = p_t.y * dab_location.z;
    p_t.x = p_t.x + dab_location.x;
    p_t.y = p_t.y + dab_location.y;
    p_t.x = ((p_t.x / scr_size.x)*2) -1;
    p_t.y = (( ( p_t.y) / scr_size.y)*2) -1;
   gl_Position = vec4( p_t.x, p_t.y,
                0.0, 
                1.0);

    uv.x = (in_Position.x/2)+0.5;
    uv.y = (in_Position.y/2)+0.5;
}
