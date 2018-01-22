#ifndef BGLPP_API_MOUSE
#define BGLPP_API_MOUSE

#include "./hwgfx/cpp_camera.h"
#include <functional>

namespace bgl {
    namespace mouse {

        enum buttons {
            left = 1,
            middle = 2, 
            right = 3
        };

        std::pair<float, float> get_worldspace( bgl::camera& camera );
        void _dispatch_motion(int x, int y);
        void _dispatch_mousedown( int button );
        void _dispatch_mouseup( int button );
        bool is_button_down( int button );

        void register_click_handler( int button, std::function<void()>);
    }
}

#endif
