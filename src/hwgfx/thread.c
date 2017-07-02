#include <SDL.h>
#include "command_message.h"

#include "blend_control.h"
#include "framebuffer.h"
#include "primitive.h"
#include "misc.h"
#include "context.h"
#include "shader.h"
#include "texture.h"

void* __structcp( void* src, size_t size) {
    void* r = malloc(size);
    memcpy(r,src,size);
    return r;
}

void GXC_FREE(void* d) {
    //would like to have a seperate thread just for disposing these dead buffer objects
    free(d);
}

char* lut[] = {
"#define GXC_BLEND_SET_MODE (0)",
"#define GXC_BLEND_EXIT (1)",
"#define GXC_FB_CREATE (2)",
"#define GXC_FB_DROP (3)",
"#define GXC_FB_BIND_TEXTURE (4)",
"#define GXC_FB_RENDER_START (5)",
"#define GXC_FB_RENDER_END (6)",
"#define GXC_CLEAR (7)",
"#define GXC_SET_CLEAR_COLOR (8)",
"#define GXC_SET_VIEWPORT (9)",
"#define GXC_CREATE_COORDINATE_PRIMITIVE (10)",
"#define GXC_DESTROY_COORDINATE_PRIMITIVE (11)",
"#define GXC_CREATE_COORDINATE_UV_PRIMITIVE (12)",
"#define GXC_RENDER_PRIMITIVE (13)",
"#define GXC_SHADER_COMPILE (14)",
"#define GXC_SHADER_BIND (15)",
"#define GXC_SHADER_BIND_FLOAT (16)",
"#define GXC_SHADER_BIND_VEC2 (17)",
"#define GXC_SHADER_BIND_VEC3 (18)",
"#define GXC_SHADER_BIND_VEC4 (19)",
"#define GXC_SHADER_BIND_INT (20)",
"#define GXC_SHADER_BIND_TEXTURE (21)",
"#define GXC_SHADER_DROP (22)",
"#define GXC_TEXTURE_FROM_SDL_SURFACE (23)",
"#define GXC_TEXTURE_BIND (24)",
"#define GXC_TEXTURE_DROP (25)",
"#define GXC_TEXTURE_GENERATE (26)",
"#define GXC_TEXTURE_GENERATE_FILTERED (27)",
"#define GXC_TEXTURE_GENERATE_FP (28)",
"#define GXC_TEXTURE_GENERATE_FP_DATA (29)",
"#define GXC_TEXTURE_GENERATE_FP_DATA_FILTERED (30)" };
void GXC_exec(gc_msg m) {

   // printf("%s\n",lut[m.cmd]);

    switch(m.cmd) {
        case GXC_BLEND_SET_MODE:
            __blend_set_mode( m.pta[0].ui );
            break;
        case GXC_BLEND_EXIT:
            _manual_blend_exit();
            break;
        case GXC_FB_CREATE:
            _framebuffer_create_framebuffer( (gfx_framebuffer*)m.pta[0].obj );
            break;
        case GXC_FB_DROP:
            _framebuffer_drop( (gfx_framebuffer*)m.pta[0].obj );
            break;
        case GXC_FB_BIND_TEXTURE:
            _framebuffer_bind_texture( (gfx_framebuffer*)m.pta[0].obj, (gfx_texture*)m.pta[1].obj );
            break;
        case GXC_FB_RENDER_START:
            _framebuffer_render_start( (gfx_framebuffer*)m.pta[0].obj );
            break;
        case GXC_FB_RENDER_END:
            _framebuffer_render_end( (gfx_framebuffer*)m.pta[0].obj );
            break;
        case GXC_CLEAR:
            _gfx_clear();
            break;
        case GXC_SET_CLEAR_COLOR:
            _gfx_set_clear_color( m.pta[0].f, m.pta[1].f, m.pta[2].f, m.pta[3].f );
            break;
        case GXC_SET_VIEWPORT:
            _gfx_viewport_set_dims( m.pta[0].i,m.pta[1].i,m.pta[2].i,m.pta[3].i );
            break;
        case GXC_CREATE_COORDINATE_PRIMITIVE:
            _primitive_create_coordinate_primitive( m.pta[0].obj, (gfx_float*)m.mma[0].obj, m.pta[1].i, m.pta[2].i );
            GXC_FREE(m.mma[0].obj);
            break;
        case GXC_DESTROY_COORDINATE_PRIMITIVE:
            _primitive_destroy_coordinate_primitive(m.mma[0].obj);
            GXC_FREE(m.mma[0].obj);
            break;
        case GXC_CREATE_COORDINATE_UV_PRIMITIVE:
            _primitive_create_coordinate_uv_primitive( m.pta[0].obj, (gfx_float*)m.mma[0].obj, (gfx_float*)m.mma[1].obj, m.pta[1].i,m.pta[2].i );
            break;
        case GXC_RENDER_PRIMITIVE:
            _primitive_render( m.pta[0].obj );
            break;
        case GXC_SHADER_COMPILE:
            _shader_compile( (gfx_shader*)m.pta[0].obj,
                            m.mma[0].str, m.mma[1].str, m.mma[2].str, m.mma[3].str );   
            GXC_FREE( m.mma[0].str );
            GXC_FREE( m.mma[1].str );
            GXC_FREE( m.mma[2].str );
            GXC_FREE( m.mma[3].str );
            break;
        case GXC_SHADER_BIND:
            _shader_bind( (gfx_shader*)m.pta[0].obj );
            break;
        case GXC_SHADER_BIND_VEC4:
	    _shader_bind_vec4((gfx_shader*)m.pta[0].obj,
                m.mma[0].str,
		m.pta[1].f,
		m.pta[2].f,
		m.pta[3].f,
		m.pta[4].f);
            GXC_FREE( m.mma[0].str );
            break;
        case GXC_SHADER_BIND_VEC3:
            _shader_bind_vec3((gfx_shader*)m.pta[0].obj,
                                m.mma[0].str,
				m.pta[1].f,
				m.pta[2].f,
				m.pta[3].f );
            GXC_FREE( m.mma[0].str );
            break;
        case GXC_SHADER_BIND_VEC2:
            _shader_bind_vec2((gfx_shader*)m.pta[0].obj,
                                m.mma[0].str,
				m.pta[1].f,
				m.pta[2].f );
            GXC_FREE( m.mma[0].str );
            break;
        case GXC_SHADER_BIND_FLOAT:
            _shader_bind_float((gfx_shader*)m.pta[0].obj,
                                m.mma[0].str,
				m.pta[1].f,
				m.pta[2].f );
            GXC_FREE( m.mma[0].str );
            break;
        case GXC_SHADER_BIND_INT:
            _shader_bind_int((gfx_shader*)m.pta[0].obj,
                                m.mma[0].str,
				m.pta[1].i );
            GXC_FREE( m.mma[0].str );
            break;
        case GXC_SHADER_BIND_TEXTURE:
            _shader_bind_texture((gfx_shader*)m.mma[0].obj,
                                m.mma[1].str,
				m.mma[2].obj );
            GXC_FREE( m.mma[0].obj );
            GXC_FREE( m.mma[1].str );
            GXC_FREE( m.mma[2].obj );
            break;
        case GXC_SHADER_DROP:
            _shader_drop( (gfx_shader*)m.mma[0].obj );
            GXC_FREE( m.mma[0].obj );
            break;
        case GXC_TEXTURE_FROM_SDL_SURFACE:
            _texture_from_SDL_surface( (gfx_texture*)m.mma[0].obj, (SDL_Surface*)m.mma[1].obj);
            GXC_FREE( m.mma[0].obj );
            //mma1 freed by texture loader
            break;
        case GXC_TEXTURE_GENERATE:
            _texture_generate( (gfx_texture*)m.pta[0].obj,m.pta[1].i,m.pta[2].i );
            break;

        case GXC_TEXTURE_GENERATE_FILTERED:
            _texture_generate_filtered( (gfx_texture*)m.pta[0].obj, m.pta[1].i, m.pta[2].i );
            break;

        case GXC_TEXTURE_GENERATE_FP:
            _texture_generate_fp( (gfx_texture*)m.pta[0].obj, m.pta[1].i, m.pta[2].i );
            break;

        case GXC_TEXTURE_GENERATE_FP_DATA:
            _texture_generate_fp_data( (gfx_texture*)m.pta[0].obj, m.pta[1].i, m.pta[2].i, (float*)m.mma[0].obj );
            GXC_FREE(m.mma[0].obj);
            break;

        case GXC_TEXTURE_GENERATE_FP_DATA_FILTERED:
            _texture_generate_fp_data_filtered( (gfx_texture*)m.pta[0].obj, m.pta[1].i, m.pta[2].i,(float*)m.mma[0].obj );
            GXC_FREE(m.mma[0].obj);
            break;
        case GXC_TEXTURE_DROP:
            _texture_drop(m.mma[0].obj);
            GXC_FREE(m.mma[0].obj);
            break;

    } 
}

void GXC_ISSUE(gc_msg m) {
    GXC_exec(m);
}
