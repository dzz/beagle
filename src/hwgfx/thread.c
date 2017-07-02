#include "command_message.h"

void GXC_exec(gc_msg m) {
    switch(m.cmd) {
        case GXC_BLEND_SET_MODE:
            __blend_set_mode( m.pta[0].ui );
            break;
        case GXC_BLEND_EXIT:
            _manual_blend_exit();
            break;
    } 
}

void GXC_ISSUE(gc_msg m) {
    GXC_exec(m);
}
