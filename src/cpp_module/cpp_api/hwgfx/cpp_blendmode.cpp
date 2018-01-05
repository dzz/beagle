#include "cpp_blendmode.h"
#include "../../../hwgfx/blend_control.h"

#include <stack>

namespace bgl {

std::stack<unsigned int> blendmode::stack;

void bgl::blendmode::use(unsigned int mode, std::function<void()> rendering_function) {

	manual_blend_enter(mode);
	blendmode::stack.push(mode);
	rendering_function();
	blendmode::stack.pop();

	if (blendmode::stack.size() > 0) {
		manual_blend_enter(blendmode::stack.top());
	}
	else {
		manual_blend_exit();
	}
}


}
