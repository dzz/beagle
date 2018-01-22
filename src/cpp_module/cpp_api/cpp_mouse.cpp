#include "cpp_mouse.h"
#include "cpp_environment.h"

namespace bgl{ namespace mouse {

static int mx = 0;
static int my= 0;

void _dispatch_motion( int x, int y ) {
    mx=x;
    my=y;
}

std::pair<float,float> get_worldspace( bgl::camera& camera ) {

    camera_data camstate = camera.get_state();
    std::pair<unsigned int, unsigned int> dims = bgl::environment::get_screen_size();

    float nx = float(mx)/float(dims.first);
    float ny = float(my)/float(dims.second);

    auto reduction = camstate.view.getReduction();

    nx*=2; nx-=1; nx/= reduction.first;
    ny*=2; ny=1-ny; ny/= reduction.second;


    nx/= camstate.zoom;
    ny/= camstate.zoom;
    nx += camstate.p.first;
    ny += camstate.p.second;

    return std::make_pair(nx,ny);
}

}}
