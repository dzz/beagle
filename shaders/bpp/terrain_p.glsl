#version 440 

in float height;
in vec2 position;
uniform sampler2D tex_water;
uniform sampler2D tex_grass;
uniform sampler2D tex_rock;
uniform sampler2D tex_alpine;

uniform float sea_level;
uniform float rock_level;
uniform float alpine_level;

void main(void) {

    float vheight = (1.0+height)/2.0;

    float tex_mod = vheight*0.02;
    vec4 s1 = texture( tex_water,  position*(0.01)+tex_mod );
    vec4 s2 = texture( tex_grass,  position*(0.01)+tex_mod );
    vec4 s3 = texture( tex_rock,   position*(0.01)+tex_mod  );
    vec4 s4 = texture( tex_alpine, position*(0.01)+tex_mod );

    vec4 color;

    if( height < sea_level ) 
        color.rgb = s1.rgb;
    else if( height < rock_level )
        color.rgb = s2.rgb;
    else if( height < alpine_level )
        color.rgb = s3.rgb;
    else
        color.rgb = s4.rgb;

    gl_FragColor.rgb = color.rgb * (0.5+(smoothstep(0.5,1.0,height*3.0)));
    gl_FragColor.a = 1.0;
}

