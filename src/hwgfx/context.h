#ifndef __GFX_CONTEXT__
#define __GFX_CONTEXT__

typedef struct {
    int x;
    int y; 
    int w;
    int h; 
} viewport_dims;

typedef struct {
    int w;
    int h;
} root_gfx_size;

void            gfx_viewport_set_dims(viewport_dims dims);
viewport_dims   gfx_viewport_get_dims();
root_gfx_size   gfx_get_root_gfx_size();

#endif
