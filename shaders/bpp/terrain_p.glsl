#version 440 

in float height;
in vec2 position;
uniform sampler2D tex_water;
uniform sampler2D tex_grass;
uniform sampler2D tex_rock;

void main(void) {

    vec4 s1 = texture( tex_water, position*0.01 );
    //vec4 s2 = texture( tex_water, position );
    vec4 s3 = texture( tex_grass, position*0.01 );


    float vheight = (1.0+height)/2.0;
    gl_FragColor = s1*height+((1.0-vheight)*s3);
}

