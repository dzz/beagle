
#ifndef OGL_LOG

#include "../system/log.h"
#include <stdio.h>

    #define OGL_RECV    "acquired"
    #define OGL_DROP    "dropped"

    //#define PRINTOGL_MSGS

    #ifdef PRINTOGL_MSGS
        #define OGL_OBJ(object,id,mode) log_message(CTT2_INT_HWGFX_OGLOBJ, LOG_LEVEL_GFXMSG, "GL:%-12s - ID:%-3x - MODE:%-10s",object,id,mode)
        #define OGL_SHADOP(vshad,fshad) log_message(CTT2_INT_HWGFX_OGLOBJ, LOG_LEVEL_GFXMSG, "GL: SHADEROPS ON: %s, %s",vshad,fshad)
    #else
        #define OGL_OBJ(object,id,mode) 
        #define OGL_SHADOP(vshad,fshad)
    #endif

#endif
