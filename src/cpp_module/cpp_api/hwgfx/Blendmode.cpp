#include "Blendmode.h"
#include "../../../hwgfx/blend_control.h"

#include <stack>
std::stack<unsigned int> Blendmode::stack;

void Blendmode::Use( unsigned int mode, std::function<void()> rendering_function) {

    manual_blend_enter(mode);
    Blendmode::stack.push(mode);
    rendering_function();
    Blendmode::stack.pop();

    if(Blendmode::stack.size()>0) {
        manual_blend_enter(Blendmode::stack.top());
    } else {
        manual_blend_exit();
    }
}
