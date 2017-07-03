#include "texture.h"
#ifndef __GFX_CMD_MESSAGE__
#define __GFX_CMD_MESSAGE__

#define GXC_BLEND_SET_MODE (0)
#define GXC_BLEND_EXIT (1)
#define GXC_FB_CREATE (2)
#define GXC_FB_DROP (3)
#define GXC_FB_BIND_TEXTURE (4)
#define GXC_FB_RENDER_START (5)
#define GXC_FB_RENDER_END (6)
#define GXC_CLEAR (7)
#define GXC_SET_CLEAR_COLOR (8)
#define GXC_SET_VIEWPORT (9)
#define GXC_CREATE_COORDINATE_PRIMITIVE (10)
#define GXC_DESTROY_COORDINATE_PRIMITIVE (11)
#define GXC_CREATE_COORDINATE_UV_PRIMITIVE (12)
#define GXC_RENDER_PRIMITIVE (13)
#define GXC_SHADER_COMPILE (14)
#define GXC_SHADER_BIND (15)
#define GXC_SHADER_BIND_FLOAT (16)
#define GXC_SHADER_BIND_VEC2 (17)
#define GXC_SHADER_BIND_VEC3 (18)
#define GXC_SHADER_BIND_VEC4 (19)
#define GXC_SHADER_BIND_INT (20)
#define GXC_SHADER_BIND_TEXTURE (21)
#define GXC_SHADER_DROP (22)
#define GXC_TEXTURE_FROM_SDL_SURFACE (23)
#define GXC_TEXTURE_BIND (24)
#define GXC_TEXTURE_DROP (25)
#define GXC_TEXTURE_GENERATE (26)
#define GXC_TEXTURE_GENERATE_FILTERED (27)
#define GXC_TEXTURE_GENERATE_FP (28)
#define GXC_TEXTURE_GENERATE_FP_DATA (29)
#define GXC_TEXTURE_GENERATE_FP_DATA_FILTERED (30)



typedef union {
    unsigned int ui;
    int i;
    void* obj;
    float f;
    double d;
    char* str; 
} gc_arg;

typedef struct {
    unsigned int cmd;
    gc_arg pta[16];
    gc_arg mma[16];
} gc_msg;


void render_thread();
void GXC_ISSUE(gc_msg m);
void GXC_exec(gc_msg m);
void GXC_free(void* gxco);

void* __structcp( void* src, size_t size);

#endif
