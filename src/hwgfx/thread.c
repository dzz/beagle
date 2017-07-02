#include "command_message.h"

#include "blend_control.h"
#include "framebuffer.h"
#include "misc.h"

char*lut[] = {
"GXC_BLEND_SET_MODE (0)",
"GXC_BLEND_EXIT (1)",
"GXC_FB_CREATE (2)",
"GXC_FB_DROP (3)",
"GXC_FB_BIND_TEXTURE (4)",
"GXC_FB_RENDER_START (5)",
"GXC_FB_RENDER_END (6)",
"GXC_CLEAR (7)",
"GXC_SET_CLEAR_COLOR(8)",
"GXC_SET_VIEWPORT (9)"
};

void GXC_exec(gc_msg m) {

    //printf("%s\n",lut[m.cmd]);

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
    } 
}

void GXC_ISSUE(gc_msg m) {
    GXC_exec(m);
}
