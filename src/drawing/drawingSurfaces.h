#ifndef __DRAWING_SURFACES__
#define __DRAWING_SURFACES__

#include "../colors/colors.h"

#include <SDL.h>

typedef SDL_Surface* DRAWING_CONTEXT;
typedef SDL_Surface* DRAWING_SURFACE;

SDL_Surface* createDrawingSurface(int w, int h);
void destroyDrawingSurface(DRAWING_SURFACE ds);
void dropDrawingSurfaces();
SDL_Surface * getConvertedForBlit(SDL_Surface *drawingContext, SDL_Surface *screenSurface);

void drawing_surface_restore_default_blending( SDL_Surface* drawingSurface);

uint_rgba_map sample_surface(SDL_Surface* ctxt, int x0, int y0);
#endif
