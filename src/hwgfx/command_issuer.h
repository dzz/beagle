#ifndef __GFX_COMMAND_ISSUER__
#define __GFX_COMMAND_ISSUER__

#define BEAGLE_OGL_ISSUE_MAXPARAMS 32

#define GFXCOMMANDMODE_IMMEDIATE              (0)
#define GFXCOMMANDMODE_DEFERRED               (1)

#define GFXCOMMAND_TEXT_RENDER                (0)
#define GFXCOMMAND_BLEND_ENTER                (1)
#define GFXCOMMAND_BLEND_EXIT                 (2)
#define GFXCOMMAND_MANUAL_BLEND_ENTER         (3)
#define GFXCOMMAND_MANUAL_BLEND_EXIT          (4)
#define GFXCOMMAND_SHADER_BIND                (5)
#define GFXCOMMAND_SHADER_BIND_FLOAT          (6)
#define GFXCOMMAND_SHADER_BIND_FLOATS         (7)
#define GFXCOMMAND_SHADER_BIND_INT            (8)
#define GFXCOMMAND_SHADER_BIND_VEC2           (9)
#define GFXCOMMAND_SHADER_BIND_VEC3           (10)
#define GFXCOMMAND_SHADER_BIND_VEC4           (11)
#define GFXCOMMAND_SHADER_BIND_TEXTURE        (12)
#define GFXCOMMAND_TEXTURE_BIND               (13)
#define GFXCOMMAND_FRAMEBUFFER_BIND_TEXTURE   (14)
#define GFXCOMMAND_VIEWPORT_SET               (15)
#define GFXCOMMAND_VIEWPORT_RESET             (16)
#define GFXCOMMAND_CLEAR                      (17)
#define GFXCOMMAND_SET_CLEAR_COLOR            (18)
#define GFXCOMMAND_FRAME_FINISHED             (19)
#define GFXCOMMAND_PRIMITIVE_RENDER           (20)

typedef union {
        int i;
        unsigned int ui;
        float f;
        double d;
        void* object;
        float* farray;
        double* darray;
        int* iarray;
        unsigned int* uiarray;
        char* str;

} gfxcommand_param;

typedef struct {
    unsigned int command;
    gfxcommand_param params[BEAGLE_OGL_ISSUE_MAXPARAMS];
} gfxcommand_call;


void gfxcommand_issue_call(gfxcommand_call gc);
void gfxcommand_renderthread();
void gfxcommand_stop();
void gfxcommand_pause();
void gfxcommand_unpause();

#endif
