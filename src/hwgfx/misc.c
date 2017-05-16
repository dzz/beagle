#include <GLXW/glxw.h>
#include "misc.h"

void gfx_clear(){
    glClear(GL_COLOR_BUFFER_BIT);
}

void gfx_set_clear_color(float r, float g, float b, float a) {
    glClearColor(r,g,b,a);
}
