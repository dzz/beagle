#include "cpp_mouse.h"
#include "cpp_environment.h"
#include <unordered_map>

namespace bgl{ namespace mouse {

static int mx = 0;
static int my= 0;

static std::unordered_map<int, bool> button_states;
static std::unordered_map<unsigned int, std::function<void()>> button_click_handlers;

void _dispatch_mousedown( int button ) {
    button_states[button] = true;
}

void _dispatch_mouseup( int button ) {

    printf("button %d\n", button);

    if(is_button_down(button)) {
        if(button_click_handlers.find(button)!=button_click_handlers.end()) {
            button_click_handlers[button]();
        }
    }
    button_states[button] = false;
}

bool is_button_down( int button ) {
    if(button_states.find(button) == button_states.end())
        return false;
    return button_states[button];
}

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

void register_click_handler( int button, std::function<void()> handler) {
    button_click_handlers[button] = handler;
}

}}
