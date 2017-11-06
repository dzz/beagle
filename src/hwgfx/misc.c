
#include "../memory.h"
#include <GLXW/glxw.h>
#include "misc.h"
#include "command_message.h"

void _gfx_clear(){
    glClear(GL_COLOR_BUFFER_BIT);
}

void gfx_clear() {
    gc_msg m;
    m.cmd = GXC_CLEAR;
    GXC_ISSUE(m);
}

void _gfx_set_clear_color(float r, float g, float b, float a) {
    //printf("ACTUAL : %f %f %f %f\n", r,g,b,a );
    glClearColor(r,g,b,a);
}

void gfx_set_clear_color(float r, float g, float b, float a) {
    gc_msg m;
    m.cmd = GXC_SET_CLEAR_COLOR;
    m.pta[0].f = (float)(double)r;
    m.pta[1].f = (float)(double)g;
    m.pta[2].f = (float)(double)b;
    m.pta[3].f = (float)(double)a;

    GXC_ISSUE(m);
}
