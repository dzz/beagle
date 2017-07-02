#ifndef __GFX_CMD_MESSAGE__
#define __GFX_CMD_MESSAGE__


#define GXC_BLEND_SET_MODE (0)
#define GXC_BLEND_EXIT (1)
#define GXC_FB_CREATE (2)
#define GXC_FB_DROP (3)
#define GXC_FB_BIND_TEXTURE (4)
#define GXC_FB_RENDER_START (5)
#define GXC_FB_RENDER_END (6)


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
//void GXC_exec(gcm);

#endif
