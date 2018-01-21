#ifndef BGLPP_API_HWGFX_CAMERA
#define BGLPP_API_HWGFX_CAMERA

#include "cpp_hwgfx_config.h"
#include <utility>
#include "cpp_view.h"
#include "cpp_shader.h"

namespace bgl{

struct camera_data {
  std::pair<float,float> p;
  float zoom;
  bgl::view view;
};

class camera {

    public:
	camera( std::pair<float,float> p = std::pair<float,float>(0.0f,0.0f) , float zoom = 1.0f, bgl::view view = bgl::view::centered_view(16,9, bgl::axis::Y_AXIS_DOWN ) );
    void update( std::pair<float,float> new_p, float new_zoom );
    void update( camera_data& data );
    const void bind( bgl::shader::vf_shader& shader);
    void move( std::pair<float,float> delta );
    camera_data get_state();

    private:
        std::pair<float,float> p;
        float zoom;
        bgl::view view;
        
};

};
#endif
