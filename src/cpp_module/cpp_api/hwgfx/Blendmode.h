#ifndef BGLPP_API_HWGFX_BLEND_H
#define BGLPP_API_HWGFX_BLEND_H

#include <stack>
#include <functional>
#include "../../../hwgfx/blend_control.h"

class Blendmode {
    public:
        static void Use(unsigned int mode, std::function<void()> rendering_function);
    private:
        static std::stack<unsigned int> stack;
};

#endif

