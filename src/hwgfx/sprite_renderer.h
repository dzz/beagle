#ifndef BGL_SHIM_SPRITE_RENDERER
#define BGL_SHIM_SPRITE_RENDERER

#include "./texture.h"

#ifdef __cplusplus
extern "C" {
#endif
    void initSpriteRenderer();
    void SR_add_sprite( float z_index, float tlx, float tly, float slx, float sly, float rad, float twx, float twy, float swx, float swy, float fr, float fg, float fb, float fa, float fcr, float fcg, float fcb, float fca, gfx_texture* texture ); 
    void SR_render(float view_x, float view_y);
    void dropSpriteRenderer();   
#ifdef __cplusplus
}
#endif


#endif
