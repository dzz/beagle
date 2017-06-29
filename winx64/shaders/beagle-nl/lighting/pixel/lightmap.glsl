#version 330 core

in vec2 uv;
uniform vec2 position;
uniform vec2 view;
uniform vec2 translation_world;
uniform vec2 scale_world;
uniform vec4 light_color;
uniform int num_p;
uniform int num_lines;
uniform float light_radius;
uniform sampler2D transformed_lines;

float RayToLineSegment(float x, float y, float dx, float dy, float x1, float y1, float x2, float y2)
{
    float r;
    float s;
    float d;

    if (dy / dx != (y2 - y1) / (x2 - x1))
    {
        d = ((dx * (y2 - y1)) - dy * (x2 - x1));
        if (d != 0)
        {
            r = (((y - y1) * (x2 - x1)) - (x - x1) * (y2 - y1)) / d;
            s = (((y - y1) * dx) - (x - x1) * dy) / d;
            if (r >= 0 && s >= 0 && s <= 1)
            {
                return r;
                //return { x: x + r * dx, y: y + r * dy };
            }
        }
    }
    return 1.0;
}

void main(void) {

    vec2 mod_uv = (uv*2)-vec2(1.0,1.0);
    vec2 real_uv = (mod_uv / (view*scale_world))-translation_world;
    vec2 position_normalized = (position+translation_world) * scale_world * view;
    float dist_to_position = distance( position, real_uv );
    float pixel_sees_position = 1.0;
    //float mod_light_radius = length((vec2(light_radius*2,light_radius*2))*scale_world*view );
    float mod_light_radius = light_radius;


    if(dist_to_position>mod_light_radius) {
        pixel_sees_position = 0.0;
    } else {

            float step_size = 1.0 / 512;
            float data_x = 0.0;
            for(int idx=0; idx<num_lines;++idx) {
                vec2 data_uv = vec2(data_x, 0);
                data_x += step_size;
                vec4 encoded_line = texture(transformed_lines, data_uv);
                vec2 line_a = vec2( encoded_line.r, encoded_line.g );
                vec2 line_b = vec2( encoded_line.b, encoded_line.a );
                float intersection_distance = RayToLineSegment( position_normalized.x, position_normalized.y, mod_uv.x - position_normalized.x, mod_uv.y - position_normalized.y, line_a.x, line_a.y, line_b.x, line_b.y);
                if( intersection_distance < 1.0) {
                        pixel_sees_position = 0.0;
                        break;
                } 
            }
    }

    vec4 outputColor = light_color * (1.0-(dist_to_position/mod_light_radius)) * pixel_sees_position;

    gl_FragColor = outputColor;
}
