#ifndef __GFX_TEXTURE__
#define __GFX_TEXTURE__
#include <SDL.h>
#include <GLXW/glxw.h>

typedef struct {
    GLuint texture_id;
    int bound_unit;
} gfx_texture;


void texture_generate                   (gfx_texture* texture, int w,int h);
void texture_generate_filtered          (gfx_texture* texture, int w,int h);
void texture_generate_fp                (gfx_texture* texture, int w,int h);
void texture_from_SDL_surface           (gfx_texture* texture, SDL_Surface* surf);
void texture_from_SDL_surface_grayscale (gfx_texture* texture, SDL_Surface* surf);
void texture_drop                       (gfx_texture* texture);
void texture_bind                       (gfx_texture* texture, int texture_unit);
void texture_download                   (gfx_texture* texture, SDL_Surface* target);

#define TEX_UNIT_0 0

#endif
