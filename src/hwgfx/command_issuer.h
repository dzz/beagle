#ifndef __GFX_COMMAND_ISSUER__
#define __GFX_COMMAND_ISSUER__

#define BEAGLE_OGL_ISSUE_MAXPARAMS 32

const unsigned int GFXCOMMANDMODE_IMMEDIATE             0;
const unsigned int GFXCOMMANDMODE_DEFERRED              1;

const unsigned int GFXCOMMAND_TEXT_RENDER               0;
const unsigned int GFXCOMMAND_BLEND_ENTER               1;
const unsigned int GFXCOMMAND_BLEND_EXIT                2;
const unsigned int GFXCOMMAND_MANUAL_BLEND_ENTER        3;
const unsigned int GFXCOMMAND_MANUAL_BLEND_EXIT         4;
const unsigned int GFXCOMMAND_SHADER_BIND               5;
const unsigned int GFXCOMMAND_SHADER_BIND_FLOAT         6;
const unsigned int GFXCOMMAND_SHADER_BIND_FLOATS        7;
const unsigned int GFXCOMMAND_SHADER_BIND_INT           8;
const unsigned int GFXCOMMAND_SHADER_BIND_VEC2          9;
const unsigned int GFXCOMMAND_SHADER_BIND_VEC3          10;
const unsigned int GFXCOMMAND_SHADER_BIND_VEC4          11;
const unsigned int GFXCOMMAND_SHADER_BIND_TEXTURE       12;
const unsigned int GFXCOMMAND_TEXTURE_BIND              13;
const unsigned int GFXCOMMAND_FRAMEBUFFER_BIND_TEXTURE  14;
const unsigned int GFXCOMMAND_VIEWPORT_SET              15;
const unsigned int GFXCOMMAND_VIEWPORT_RESET            16;
const unsigned int GFXCOMMAND_CLEAR                     17;
const unsigned int GFXCOMMAND_SET_CLEAR_COLOR           18;
const unsigned int GFXCOMMAND_FRAME_FINISHED            19;

union {
        unsigned int commmand;
        int i;
        unsigned int ui;
        float f;
        double d;
        void* object;
        float* farray;
        double* darray;
        int* iarray;
        unsigned int* uiarray;

} gfxcommand_param;

typedef struct {
    unsigned int function_id;
    gfxcommand_param params[BEAGLE_OGL_ISSUE_MAXPARAMS];
} gfxcommand_call;


void gfxcommand_issue_call(gfxcommand_call gc);
void gfxcommmand_

#endif
