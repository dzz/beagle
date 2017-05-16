#ifndef __GFX_QUADS__
#define __GFX_QUADS__

#include "shader.h"

void initRects();

void rect_draw
    (int x,int y, int w, int h);

void rect_draw_tex
    (int x,int y, int w, int h,float u,float v, float tw, float th);

void dropRects();

#endif
