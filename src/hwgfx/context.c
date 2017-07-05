#include <GLXW/glxw.h>

#include "command_message.h"
#include "../system/ctt2.h"
#include "../system/log.h"
#include "context.h"

int _vp[4] = {0};

int _unknown = 1;

void _validate_vp(){
/*
    if(_unknown == 1){
        glGetIntegerv(GL_VIEWPORT,_vp);
        _unknown = 0;
    }*/
}


void _gfx_viewport_set_dims(int x,int y,int w, int h ) {
    glViewport( x, 
                y, 
                w, 
                h );
}
void gfx_viewport_set_dims(viewport_dims dims) {

    //printf("SETTING VIEWPORT");
    //exit(1);
    //log_message( CTT2_INT_HWGFX, LOG_LEVEL_GFXMSG, "VIEWPORT(%d,%d,%d,%d)",dims.x,dims.y,dims.w,dims.h); 
    //glViewport( dims.x, 
    //            dims.y, 
    //            dims.w, 
    //            dims.h );
    _unknown = 0; 
    _vp[0] = dims.x;
    _vp[1] = dims.y;
    _vp[2] = dims.w;
    _vp[3] = dims.h;

    gc_msg m;
    m.cmd = GXC_SET_VIEWPORT;
    m.pta[0].i = dims.x;
    m.pta[1].i = dims.y;
    m.pta[2].i = dims.w;
    m.pta[3].i = dims.h;

    GXC_ISSUE(m);
}

viewport_dims   gfx_viewport_get_dims() {
    viewport_dims dims;
    
    _validate_vp();
    dims.x = _vp[0];
    dims.y = _vp[1];
    dims.w = _vp[2];
    dims.h = _vp[3];
    return dims;
}


root_gfx_size
    gfx_get_root_gfx_size(){
    root_gfx_size gfx_size;
    gfx_size.w = host_get_screen_width();
    gfx_size.h = host_get_screen_height();
    return gfx_size;
}
