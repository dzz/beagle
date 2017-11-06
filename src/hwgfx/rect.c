
#include "../memory.h"
#include "context.h"
#include "shader.h"
#include "primitive.h"
#include "rect.h"

typedef struct{
    gfx_coordinate_primitive quad_primitive;
} quad_context;
static quad_context _quad_context;

void initRects(){
    primitive_create_scrquad_primitive
        (&_quad_context.quad_primitive);
}

void dropRects(){
    primitive_destroy_coordinate_primitive
        (&_quad_context.quad_primitive);
}

void rect_draw(int x,int y, int w, int h) {
    gfx_shader* bound_shader    = shader_get_bound();
    viewport_dims scr_dims      = gfx_viewport_get_dims();

    shader_bind_vec4( bound_shader, "rect_coords", 
                     (float)x,
                     (float)y,   
                     (float)w,
                     (float)h);

    shader_bind_vec2( bound_shader, "scr_size", 
                     (float)scr_dims.w,
                     (float)scr_dims.h);

    primitive_render( &_quad_context.quad_primitive);
}

void rect_draw_tex(int x,int y, int w, int h,float u,float v,float tw,float th) {
    gfx_shader* bound_shader    = shader_get_bound();
    viewport_dims scr_dims      = gfx_viewport_get_dims();

    shader_bind_vec4( bound_shader, "rect_coords", 
                     (float)x,
                     (float)y,   
                     (float)w,
                     (float)h);

    shader_bind_vec4( bound_shader, "uv_coords", 
                     (float)u,
                     (float)v,   
                     (float)tw,
                     (float)th);

    shader_bind_vec2( bound_shader, "scr_size", 
                     (float)scr_dims.w,
                     (float)scr_dims.h);

    primitive_render( &_quad_context.quad_primitive);
}
