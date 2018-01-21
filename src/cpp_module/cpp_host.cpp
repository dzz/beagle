#include <cstdio>
#include <vector>

#include "cpp_host.h"

#include "./cpp_api/hwgfx/cpp_blendmode.h"
#include "./cpp_api/hwgfx/cpp_context.h"
#include "../hwgfx/text.h"
#include "../hwgfx/blend_control.h"

#include "./flatearth/game.h"
#include "./flatearth/resources/tree.h"

extern "C" {
    void hwgfx_render_test();
}

Game * game;
int cpp_api_init() {
    puts("init\n");
    game = new Game();
    game->stage.init_test_data();
    return API_NOFAILURE;
}
int cpp_api_tick() {
    puts("tick\n");
    game->tick();
    return API_NOFAILURE;
}
int cpp_api_render() {
    puts("render\n");
    bgl::context::clear(0.01,0.01,0.01,0.0);

    //testing quadtree  render
    game->stage.resources_quad.view();
    game->stage.buildings_quad.view();

    game->view();
    return API_NOFAILURE;
}
int cpp_api_drop() {
    puts("drop\n");
    delete game;
    return API_NOFAILURE;
}
int cpp_api_dispatch_mouseup(int button, int x, int y) {
    puts("dispatch mouseup\n");
    return API_NOFAILURE;
}
int cpp_api_dispatch_mousedown(int button, int x, int y) {
    puts("dispatch mousedown\n");
    std::vector<Quadable *> v;
    game->stage.resources_quad.get_boxes(v, x, y);
    for (auto & p : v) {
        ((Tree*)p)->r = 1.0;
        ((Tree*)p)->g = 0.0;
        ((Tree*)p)->b = 1.0;
    }
    return API_NOFAILURE;
}
int cpp_api_dispatch_mousemotion(int x, int y) {
    puts("dispatch mousemotion\n");
    return API_NOFAILURE;
}
int cpp_api_dispatch_mousewheel(int y) {
    puts("dispatch mousewheel\n");
    return API_NOFAILURE;
}
int cpp_api_dispatch_key(int key, int mode) {
    puts("dispatch key\n");
    return API_NOFAILURE;
}
int cpp_api_dispatch_text(char* text) {
    puts("dispatch text\n");
    return API_NOFAILURE;
}
int cpp_api_set_screensize(int w, int h) {
    puts("set screensize\n");
    return API_NOFAILURE;
}
int cpp_api_map_keycode(char* code_definition) {
    puts("map keycode\n");
    return API_NOFAILURE;
}
