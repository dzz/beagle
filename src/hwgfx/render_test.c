
#include "../memory.h"
#include "context.h"
#include "primitive.h"
#include "shader.h"
#include "text.h"
#include "blend_control.h"
#include "framebuffer.h"
#include "misc.h"

static double u_time = 0.0;

void hwgfx_render_test() {

    const float verts[3][2] = {
        { -1.0f, -1.0f },
        { 1.0f,-1.0f},
        { 1.0f,1.0f}
    };
    const float uvs[3][2] = {
        { 0.0f, 0.0f },
        { 1.0f,0.0f},
        { 1.0f,1.0f}
    };

    gfx_set_clear_color(0.0,0.0,0.0,1.0);
    gfx_clear();

    // render a triangle

    {
        u_time = u_time + 0.01;
        gfx_coordinate_uv_primitive* primitive = malloc(sizeof(gfx_coordinate_uv_primitive)); // caller allocates, destroy calls will free on the gfx thread
        gfx_shader* shader = malloc(sizeof(gfx_shader));

        primitive_create_coordinate_uv_primitive( primitive, (gfx_float*)verts, (gfx_float*)uvs, 3, 2);
        shader_load(shader, "shaders/test/vert.glsl", "shaders/test/pixel.glsl"); 
        shader_bind(shader);
        shader_bind_float(shader, "u_time", u_time );

        primitive_render( primitive );
        primitive_destroy_coordinate_uv_primitive( primitive );

        shader_drop(shader);
    }

    // use a framebufer

    {
        gfx_texture* texture = malloc(sizeof(gfx_texture));
        gfx_framebuffer *framebuffer = malloc(sizeof(gfx_framebuffer));
        viewport_dims dims, root_dims;
        root_gfx_size rgs;


        rgs = gfx_get_root_gfx_size();
        dims.x = 0;
        dims.y = 0;
        dims.w = 16;
        dims.h = 16;

        root_dims.x = 0;
        root_dims.y = 0;
        root_dims.w = rgs.w;
        root_dims.h = rgs.h;

        texture_generate( texture, 16,16 );

        framebuffer_create_framebuffer(framebuffer);
        framebuffer_bind_texture(framebuffer, texture );

        framebuffer_render_start(framebuffer);
        {
            gfx_set_clear_color(0.2,0.2,0.2,1.0);
            gfx_clear();
            gfx_viewport_set_dims(dims);
            manual_blend_enter(0);
            text_render( 0.0,0.0,1.0,0.0,0.0,"X");
            text_render( 8.0,0.0,1.0,0.0,0.0,"O");
            text_render( 0.0,8.0,0.0,0.0,1.0,"O");
            text_render( 8.0,8.0,0.0,0.0,1.0,"X");
            manual_blend_exit();
        }
        framebuffer_render_end(framebuffer);
        gfx_viewport_set_dims(root_dims);

        {
            gfx_coordinate_uv_primitive* primitive = malloc(sizeof(gfx_coordinate_uv_primitive)); // caller allocates, destroy calls will free on the gfx thread
            gfx_shader* shader = malloc(sizeof(gfx_shader));

            primitive_create_coordinate_uv_primitive( primitive, (gfx_float*)verts, (gfx_float*)uvs, 3, 2);
            shader_load(shader, "shaders/test/vert_fb.glsl", "shaders/test/pixel_fb.glsl"); 
            shader_bind(shader);
            shader_bind_float(shader, "u_time", u_time );
            texture_bind(texture, 1);
            shader_bind_texture(shader, "texBuffer", texture );
            primitive_render( primitive );
            primitive_destroy_coordinate_uv_primitive( primitive );

            shader_drop(shader);
        }

        texture_drop(texture);
        framebuffer_drop(framebuffer);
    }

    //text
    {
        manual_blend_enter(0);
        for(int i=0; i< 24; ++i) {
            text_render( i*16.0f,i*8.0f,0.0,0.0,1.0,"X");
        }
        manual_blend_exit();
    }
}
