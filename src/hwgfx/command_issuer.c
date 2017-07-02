#include "command_issuer.h"

#include "shader.h"
#include "label.h"
#include "rect.h"
#include "blend_control.h"
#include "texture.h"
#include "primitive.h"
#include "framebuffer.h"
#include "text.h"
#include "misc.h"
#include "context.h"
#include "command_issuer.h"

#define MAX_BUFFERED_COMMANDS (16384)

extern void updateViewingSurface();

unsigned int gfxcommand_mode = GFXCOMMANDMODE_IMMEDIATE;

static unsigned int command_writeptr = 0;
static unsigned int command_readptr = 0;

gfxcommand_call command_buffer[MAX_BUFFERED_COMMANDS];

void gfxcommand_issue_call(gfxcommand_call gc) {
    if(command_writeptr> MAX_BUFFERED_COMMANDS) {
        command_writeptr = 0;
    }
     
    command_buffer[command_writeptr] = gc;
    command_writeptr += 1;

    if(command_writeptr >= MAX_BUFFERED_COMMANDS) {
        command_writeptr = 0;
    }
}

unsigned int stopped = 0;
unsigned int paused = 1;
void gfxcommand_stop() {
    stopped = 1;
}

void gfxcommand_pause() {
    paused = 1;
}

void gfxcommand_unpause() {
    paused = 0;
}



void gfxcommand_execute_call(gfxcommand_call gc) {
    switch(gc.command) {
        case GFXCOMMAND_TEXT_RENDER:
            //text_render(gc.params[0],gc.params[1],gc.params[2],gc.params[3],gc.params[4],gc.params[5]);
        break;
        case GFXCOMMAND_BLEND_ENTER:
            blend_enter(gc.params[0].ui);
        break;
        case GFXCOMMAND_BLEND_EXIT:
            blend_exit();
        break;
        case GFXCOMMAND_MANUAL_BLEND_ENTER:
            manual_blend_enter(gc.params[0].ui);
        break;
        case GFXCOMMAND_MANUAL_BLEND_EXIT:
            manual_blend_exit();
        break;
        case GFXCOMMAND_SHADER_BIND:
            shader_bind((gfx_shader*)gc.params[0].object);
        break;
        case GFXCOMMAND_SHADER_BIND_FLOAT:
            shader_bind_float((gfx_shader*)gc.params[0].object,gc.params[1].str,gc.params[2].f);
        break;
        case GFXCOMMAND_SHADER_BIND_FLOATS:
            //...........
        break;
        case GFXCOMMAND_SHADER_BIND_INT:
            shader_bind_float((gfx_shader*)gc.params[0].object,gc.params[1].str,gc.params[2].i);
        break;
        case GFXCOMMAND_SHADER_BIND_VEC2:
            shader_bind_vec2((gfx_shader*)gc.params[0].object,gc.params[1].str,gc.params[2].f,gc.params[3].f);
        break;
        case GFXCOMMAND_SHADER_BIND_VEC3:
            shader_bind_vec3((gfx_shader*)gc.params[0].object,gc.params[1].str,gc.params[2].f,gc.params[3].f,gc.params[4].f);
        break;
        case GFXCOMMAND_SHADER_BIND_VEC4:
            shader_bind_vec4((gfx_shader*)gc.params[0].object,gc.params[1].str,gc.params[2].f,gc.params[3].f,gc.params[4].f,gc.params[5].f);
        break;
        case GFXCOMMAND_SHADER_BIND_TEXTURE:
            shader_bind_texture((gfx_shader*)gc.params[0].object, gc.params[1].str,(gfx_texture*)gc.params[1].object);
        break;
        case GFXCOMMAND_TEXTURE_BIND:
            texture_bind((gfx_texture*)gc.params[0].object, gc.params[1].ui);
        break;
        case GFXCOMMAND_FRAMEBUFFER_BIND_TEXTURE:
            framebuffer_bind_texture((gfx_framebuffer*)gc.params[0].object,(gfx_texture*)gc.params[1].object);
        break;
        case GFXCOMMAND_VIEWPORT_SET:
        break;
        case GFXCOMMAND_VIEWPORT_RESET:
        break;
        case GFXCOMMAND_CLEAR:
        break;
        case GFXCOMMAND_SET_CLEAR_COLOR:
        break;
        case GFXCOMMAND_PRIMITIVE_RENDER:
        break;
        case GFXCOMMAND_FRAME_FINISHED:
            updateViewingSurface();
        break;
    }
}

void gfxcommand_renderthread() {
	while (stopped == 0) {
		if (!paused) {
			if (!command_writeptr == command_readptr) {
				gfxcommand_call cur_cmd = command_buffer[command_readptr];
				gfxcommand_execute_call(cur_cmd);
				command_readptr = command_readptr + 1;
				if (command_readptr >= MAX_BUFFERED_COMMANDS) {
					command_readptr = 0;
				}
			}
		}
	}
}

