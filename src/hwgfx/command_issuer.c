#include <stdlib.h>
#include <cstdio.h>
#include "hwgfx/command_issuer.h"

#define MAX_BUFFERED_COMMANDS 16384

static unsigned int current_command = 0;
static gfxcommand_call command_buffer[MAX_BUFFERED_COMMANDS];

void gfxcommand_issue_call(gfxcommand_call gc) {
    if(current_command> MAX_BUFFERED_COMMANDS) {
        printf("MAXIMUM GRAPHICS COMMANDS PER FRAME EXCEEDED. HARD FAIL!!!!");
        exit(1);
    }
     
    command_buffer[current_command] = gc;
    current_command += 1;
}

void gfxcommand_execute_call(gfxcommand_call gc) {

    switch(gc.command) {
        case GFXCOMMAND_TEXT_RENDER:
        break;
        case GFXCOMMAND_BLEND_ENTER:
        break;
        case GFXCOMMAND_BLEND_EXIT:
        break;
        case GFXCOMMAND_MANUAL_BLEND_ENTER:
        break;
        case GFXCOMMAND_MANUAL_BLEND_EXIT:
        break;
        case GFXCOMMAND_SHADER_BIND:
        break;
        case GFXCOMMAND_SHADER_BIND_FLOAT:
        break;
        case GFXCOMMAND_SHADER_BIND_FLOATS:
        break;
        case GFXCOMMAND_SHADER_BIND_INT:
        break;
        case GFXCOMMAND_SHADER_BIND_VEC2:
        break;
        case GFXCOMMAND_SHADER_BIND_VEC3:
        break;
        case GFXCOMMAND_SHADER_BIND_VEC4:
        break;
        case GFXCOMMAND_SHADER_BIND_TEXTURE:
        break;
        case GFXCOMMAND_TEXTURE_BIND:
        break;
        case GFXCOMMAND_FRAMEBUFFER_BIND_TEXTURE:
        break;
        case GFXCOMMAND_VIEWPORT_SET:
        break;
        case GFXCOMMAND_VIEWPORT_RESET:
        break;
        case GFXCOMMAND_CLEAR:
        break;
        case GFXCOMMAND_SET_CLEAR_COLOR:
        break;
        case GFXCOMMAND_FRAME_FINISHED:
        break;
    }
}

