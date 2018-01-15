#include "cpp_camera.h"

namespace bgl{

camera::camera( std::pair<float,float> p, float zoom, bgl::view view): p(p), zoom(zoom), view(view) { }

void camera::update( std::pair<float,float> p, float zoom, bgl::view view ) {
    p = p;
    zoom = zoom;
    view = view;
}

void camera::bind( shader::vf_shader& shader) {
	shader.str_bind_vec2( "camera_view", view.getReduction() );
	shader.str_bind_vec2( "camera_position", p );
	shader.str_bind_float( "camera_zoom", zoom );
}


}
