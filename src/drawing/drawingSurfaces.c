#include <stdio.h>

#include "drawingSurfaces.h"

SDL_Surface *conversion_buffer = 0;

//32 bit surface
SDL_Surface* createDrawingSurface(int w, int h) {
        Uint32 rmask,gmask,bmask,amask;
        SDL_Surface * canvas;

        rmask = 0xff000000;
        gmask = 0x00ff0000;
        bmask = 0x0000ff00;
        amask = 0x000000ff;

    /*    canvas = SDL_CreateRGBSurface(0, 1920,1080,32,rmask,gmask,bmask,amask);*/

        canvas = SDL_CreateRGBSurface(0, w,h,32,rmask,gmask,bmask,amask);


        SDL_FillRect( canvas, NULL, SDL_MapRGBA( canvas->format, 0x00, 0x00, 0x00, 0x00 ) );
        SDL_SetSurfaceBlendMode(canvas, SDL_BLENDMODE_NONE);
        return canvas;
}

void drawing_surface_restore_default_blending( SDL_Surface* drawingSurface) {
        SDL_SetSurfaceBlendMode(drawingSurface, SDL_BLENDMODE_NONE);
}

SDL_Surface * getConvertedForBlit(SDL_Surface *drawingContext, SDL_Surface *screenSurface) {
    if(conversion_buffer != 0) {
        SDL_FreeSurface(conversion_buffer);
    }
    conversion_buffer = SDL_ConvertSurface(drawingContext, screenSurface->format, 0);
    return conversion_buffer;
}

void dropDrawingSurfaces() {
    if(conversion_buffer == 0) {
        SDL_FreeSurface(conversion_buffer);
    }
}

void destroyDrawingSurface(DRAWING_SURFACE ds) {
    SDL_FreeSurface(ds);
}

uint_rgba_map sample_surface(SDL_Surface* ctxt, int x0, int y0) {
        unsigned int sample_a = x0+(y0*ctxt->w);
        unsigned int * pixels = ctxt->pixels;
        uint_rgba_map sample;

        if(sample_a>0) {
                sample.packed = pixels[sample_a];
        }
        return sample;
}
