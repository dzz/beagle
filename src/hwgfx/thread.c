#include "command_message.h"

#include "blend_control.h"
#include "framebuffer.h"

char*lut[] = {
"GXC_BLEND_SET_MODE (0)",
"GXC_BLEND_EXIT (1)",
"GXC_FB_CREATE (2)",
"GXC_FB_DROP (3)",
"GXC_FB_BIND_TEXTURE (4)",
"GXC_FB_RENDER_START (5)",
"GXC_FB_RENDER_END (6)"
};

void GXC_exec(gc_msg m) {

    printf("%s\n",lut[m.cmd]);

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
    } 
}

void GXC_ISSUE(gc_msg m) {
    GXC_exec(m);
}
