
#include "../memory.h"
#include "context.h"
#include "primitive.h"
#include "shader.h"
#include "texture.h"
#include "text.h"
#include "blend_control.h"
#include "framebuffer.h"
#include "misc.h"
#include "pointlight.h"

extern gfx_texture* text_get_texture();
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
    const float colors[3][4] = {
        { 1.0f, 0.0f, 0.0f, 1.0f },
        { 0.0f, 1.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 1.0f, 1.0f }
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

    //use a multichannel primitive
    {
        gfx_channel_primitive* primitive = malloc(sizeof(gfx_channel_primitive));
        gfx_shader* shader = malloc(sizeof(gfx_shader));

        gfx_float** channels = malloc(sizeof(gfx_float*)*3);
        int*    chan_lens = malloc(sizeof(int)*3);
        
        channels[0] = malloc(sizeof(gfx_float)*2*3);
        channels[1] = malloc(sizeof(gfx_float)*2*3);
        channels[2] = malloc(sizeof(gfx_float)*4*3);

        chan_lens[0] = 2;
        chan_lens[1] = 2;
        chan_lens[2] = 4;
        //verts
        channels[0][0] = -1.0f;
        channels[0][1] = -1.0f;

        channels[0][2] = 1.0f;
        channels[0][3] = -1.0f;

        channels[0][4] = 1.0f;
        channels[0][5] = 1.0f;

        //uvs
        channels[1][0] = 0.0f;
        channels[1][1] = 0.0f;

        channels[1][2] = 1.0f;
        channels[1][3] = 0.0f;

        channels[1][4] = 1.0f;
        channels[1][5] = 1.0f;

        //colors
        channels[2][0] = 1.0f;
        channels[2][1] = 0.0f;
        channels[2][2] = 0.0f;
        channels[2][3] = 1.0f;

        channels[2][4] = 0.0f;
        channels[2][5] = 1.0f;
        channels[2][6] = 0.0f;
        channels[2][7] = 1.0f;

        channels[2][8] = 0.0f;
        channels[2][9] = 0.0f;
        channels[2][10] = 1.0f;
        channels[2][11] = 1.0f;


        primitive_create_channel_primitive( primitive, 3, channels, chan_lens, 3 );
        shader_load(shader, "shaders/test/vert_chan.glsl", "shaders/test/pixel_chan.glsl"); 
        shader_bind(shader);
        shader_bind_float(shader, "u_time", u_time*0.2 );
        texture_bind(text_get_texture(), 1);
        shader_bind_texture(shader, "texBuffer", text_get_texture() );
        primitive_render( primitive );

        shader_drop(shader);
        primitive_destroy_channel_primitive( primitive );
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
            for(int j=0; j< 24; ++j) {
            text_render( i*16.0f,(i*8.0f)+(float)j,(float)j/24.0f,0.0,1.0,"X");
            }
        }
        manual_blend_exit();
    }

    //pointlight

    {
        manual_blend_enter(0);
        //shader_load(shader, "shaders/test/pointlight_vert.glsl", "shaders/test/pointlight_pixel.glsl"); 
        testPointLightRender();
        manual_blend_exit();
    }

}
