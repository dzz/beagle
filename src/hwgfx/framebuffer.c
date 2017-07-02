#include <GLXW/glxw.h>

#include "framebuffer.h"
#include "OGL_OBJ.h"

void framebuffer_create_framebuffer(gfx_framebuffer* framebuffer) {
    glGenFramebuffers(1, &framebuffer->framebuffer_id);
    OGL_OBJ("framebuffer",framebuffer->framebuffer_id,OGL_RECV);
}

void framebuffer_drop(gfx_framebuffer* framebuffer) {
    glDeleteFramebuffers(1, &framebuffer->framebuffer_id);
    OGL_OBJ("framebuffer",framebuffer->framebuffer_id,OGL_DROP);
}

#define NO_FRAMEBUFFER_BOUND 0
void framebuffer_bind_texture(gfx_framebuffer* framebuffer, gfx_texture* texture) {
#define NUM_BUFS 1
    GLenum DrawBuffers[NUM_BUFS] = {GL_COLOR_ATTACHMENT0};
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->framebuffer_id );
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture->texture_id,0);
    glDrawBuffers(NUM_BUFS, DrawBuffers); 
    glBindFramebuffer(GL_FRAMEBUFFER, NO_FRAMEBUFFER_BOUND );
}

void framebuffer_render_start(gfx_framebuffer* framebuffer) {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->framebuffer_id );
}

void framebuffer_render_end(gfx_framebuffer* framebuffer) {
    glBindFramebuffer(GL_FRAMEBUFFER, NO_FRAMEBUFFER_BOUND );
}
