#ifndef __GFX_LABEL__
#define __GFX_LABEL__

#include "primitive.h"
#include "texture.h"
#include <stdio.h>

typedef struct {
    gfx_texture *texture;
    gfx_coordinate_uv_primitive *primitive;
    int w;
    int h; 
    int _cursor; 
    int _set;
} gfx_label;

void label_generate(gfx_label* label);
void label_set_text(gfx_label* label, const char* text);
void label_drop(gfx_label* label);
void label_render(gfx_label* label, float x, float y,float r, float g, float b);
void initLabels();
void dropLabels();
int  api_host_get_char_dims();

#endif
