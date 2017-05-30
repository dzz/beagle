#version 330

// @description: composite the floor together

uniform vec2 camera_position;

uniform sampler2D floor_buffer;
uniform sampler2D light_buffer;
uniform sampler2D object_buffer;
uniform sampler2D vision_buffer;
uniform sampler2D height_buffer;
uniform sampler2D reflect_buffer;
uniform sampler2D reflect_map;
in vec2 uv;

void main(void) {

    vec2 centered_uv = (uv+vec2(-0.5,-0.5))*2;
    centered_uv*= centered_uv;

    vec2 par_mod = camera_position * 0.01 * vec2(1.0,-1.0);

    vec4 height_texel = texture(height_buffer, uv);
    vec4 floor_texel = texture(floor_buffer,uv);
    vec4 light_texel = texture(light_buffer,uv - (vec2( height_texel.r, height_texel.r)*0.01));
    vec4 object_texel = texture(object_buffer, uv);
    vec4 vision_texel = texture(vision_buffer, uv + (vec2( height_texel.r - 0.5, height_texel.r - 0.5)*0.05));
    vec4 reflect_texel = texture(reflect_buffer, uv );
    
    vec4 reflect_map_texel = texture(reflect_map, centered_uv + (par_mod*-1) + vec2(height_texel.r, height_texel.r)*0.2);

    // these are just some basics, to be parameterized and tweaked in the future 

    light_texel = light_texel * light_texel;
    vec4 lit_floor = ((light_texel*floor_texel)*1.0)*(1.0-object_texel.a);
    vec4 lit_object = (object_texel*0.7) + (object_texel*light_texel*0.3);

    lit_object.a = 1.0;

    vec4 lit_reflection = reflect_texel * light_texel * reflect_map_texel;

    lit_floor = lit_floor + lit_reflection;
    gl_FragColor = (lit_floor + lit_object) * vision_texel;

    
    //gl_FragColor = reflect_map_texel;
}

