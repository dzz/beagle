#ifndef BGLPP_API_HWGFX_BLEND_H
#define BGLPP_API_HWGFX_BLEND_H

#include "./cpp_hwgfx_config.h"
#include <stack>
#include <functional>
#include "../../../hwgfx/blend_control.h"

namespace bgl {
    class blendmode {
        public:
            static void use(unsigned int mode, std::function<void()> rendering_function);
        private:
            static std::stack<unsigned int> stack;
    };
}

#endif

