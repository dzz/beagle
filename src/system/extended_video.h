#ifndef __EXTENDED_VIDEO__
#define __EXTENDED_VIDEO__

#include <SDL.h>

void initExtendedVideo();
void dropExtendedVideo();
void resizeExtendedVideo();

void initGLExtensions();
void gfx_surface_render( SDL_Surface* img);

#endif
