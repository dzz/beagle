#ifndef BGLPP_API_HWGFX_CAMERA
#define BGLPP_API_HWGFX_CAMERA

#include "cpp_hwgfx_config.h"
#include <utility>
#include "cpp_view.h"
#include "cpp_shader.h"

namespace bgl{

class camera {

    public:
	camera( std::pair<float,float> p = std::pair<float,float>(0.0f,0.0f) , float zoom = 1.0f, bgl::view view = bgl::view(16,9) );
	void update( std::pair<float,float> p, float zoom, bgl::view view );
    void bind( bgl::shader::vf_shader& shader);

    private:
        std::pair<float,float> p;
        float zoom;
        bgl::view view;

        
};

};
#endif
