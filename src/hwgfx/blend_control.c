#include <stdlib.h>
#include <stdio.h>
#include <GLXW/glxw.h>
#include "../system/log.h"
#include "../system/rt_module_codes.h"
#include "blend_control.h"

unsigned static int _blending = 0;
unsigned static int _current_mode = -1;

void _blend_set_mode(unsigned int mode) {
    if(mode == _current_mode)
        return;

    if(_current_mode == -1)
        glEnable(GL_BLEND);

    _current_mode = mode;
    switch(mode) {
        case BLENDMODE_OVER:
               glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
               glBlendEquation(GL_FUNC_ADD);
            break;
        case BLENDMODE_BRUSH_COMPOSITE:
               glBlendFuncSeparate(GL_ONE,GL_ONE_MINUS_SRC_ALPHA,GL_ONE,GL_ONE);
               glBlendEquationSeparate(GL_FUNC_ADD,GL_FUNC_ADD);
            break;
        case BLENDMODE_BLACKHOLE:
               glBlendFuncSeparate(GL_ONE_MINUS_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
                       GL_ONE,GL_ONE);
               glBlendEquation(GL_FUNC_ADD);
            break;
        case BLENDMODE_DAB_RENDERING:
               glBlendFuncSeparate(GL_SRC_ALPHA, GL_ZERO,
                       GL_ONE,GL_ONE);
               glBlendEquationSeparate(GL_FUNC_ADD,GL_MAX);
            break;
        case BLENDMODE_ADD:
               glBlendFunc(GL_ONE, GL_ONE);
               glBlendEquation(GL_FUNC_ADD);
               break;
        case BLENDMODE_DARKEN:
               glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_COLOR);
               glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
               break;
        case BLENDMODE_DARKEN2:
               glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
               glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
               break;

    }
}
void manual_blend_enter(unsigned int mode) {
    _blend_set_mode(mode);
}

void manual_blend_exit() {
    glDisable(GL_BLEND);
    _current_mode = -1;
}

void blend_enter(unsigned int mode) {
    if(_blending == 1) {
        log_message(CTT2_INT_HWGFX,LOG_LEVEL_WARNING,"already blending");
    }
    _blending = 1;
    _blend_set_mode(mode);
    glEnable(GL_BLEND);
}


void blend_exit() {
    if(_blending !=1) {
        log_message(CTT2_INT_HWGFX,LOG_LEVEL_WARNING,"not blending, can't exit...");
        exit(1);
    }
    glDisable(GL_BLEND);
    _blending = 0;
    _current_mode = -1;
}
