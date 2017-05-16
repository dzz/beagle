#ifndef __GFX_FRAME_BUFFER__
#define __GFX_FRAME_BUFFER__

#include <GLXW/glxw.h>
#include "texture.h"

typedef struct {
    GLuint framebuffer_id;
} gfx_framebuffer;

void framebuffer_create_framebuffer(gfx_framebuffer* framebuffer);
void framebuffer_drop(gfx_framebuffer* framebuffer);
void framebuffer_bind_texture(gfx_framebuffer* framebuffer, gfx_texture* texture);
void framebuffer_render_start(gfx_framebuffer* framebuffer);
void framebuffer_render_end(gfx_framebuffer* framebuffer);

#endif
