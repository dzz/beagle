#ifndef __GFX_TEXT__
#define __GFX_TEXT__

//#include "texture.h"

#ifdef __cplusplus
extern "C" {
#endif

void initText();
void dropText();

void text_render(float x, float y, float r, float g, float b, const char* text );

//gfx_texture* text_get_texture();

#ifdef __cplusplus
}
#endif

#endif
