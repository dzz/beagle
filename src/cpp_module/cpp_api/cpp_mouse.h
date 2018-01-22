#ifndef BGLPP_API_MOUSE
#define BGLPP_API_MOUSE

#include "./hwgfx/cpp_camera.h"

namespace bgl {
    namespace mouse {
        std::pair<float, float> get_worldspace( bgl::camera& camera );
        void _dispatch_motion(int x, int y);
    }
}

#endif
