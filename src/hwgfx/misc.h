#ifndef __GFX_MISC__
#define __GFX_MISC__

#ifdef __cplusplus
extern "C" {
#endif

void gfx_clear();
void _gfx_clear();
void gfx_set_clear_color(float r, float g, float b, float a);
void _gfx_set_clear_color(float r, float g, float b, float a);

#ifdef __cplusplus
}
#endif

#endif
