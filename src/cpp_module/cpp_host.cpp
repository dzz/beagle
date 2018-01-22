#include <cstdio>
#include <vector>

#include "cpp_host.h"

#include "./cpp_api/hwgfx/cpp_blendmode.h"
#include "./cpp_api/hwgfx/cpp_context.h"
#include "../hwgfx/text.h"
#include "../hwgfx/blend_control.h"
#include "./flatearth/game.h"
#include "./flatearth/resources/tree.h"
#include "./cpp_feature/terrain/bgl_terrain.h"
#include "./cpp_api/cpp_keyboard.h"
#include "./cpp_api/cpp_mouse.h"
#include "./CPPGFXTest.h"

extern "C" {
    void hwgfx_render_test();
}

Game * game;

CPPGFXTest* GraphicsTest;

int cpp_api_init() {
    GraphicsTest = new CPPGFXTest();
    game = new Game();
    game->stage.init_test_data();
    return API_NOFAILURE;
}
int cpp_api_tick() {
    GraphicsTest->tick();
    game->tick();
    return API_NOFAILURE;
}
int cpp_api_drop() {
    delete GraphicsTest;
    delete game;
    return API_NOFAILURE;
}

int cpp_api_dispatch_mouseup(int button, int x, int y) {
    bgl::mouse::_dispatch_mouseup( button );
    return API_NOFAILURE;
}

int cpp_api_dispatch_mousedown(int button, int x, int y) {
    bgl::mouse::_dispatch_mousedown( button );
    return API_NOFAILURE;
}
int cpp_api_dispatch_mousemotion(int x, int y) {
    bgl::mouse::_dispatch_motion( x, y );
    return API_NOFAILURE;
}
int cpp_api_dispatch_mousewheel(int y) {
    return API_NOFAILURE;
}
int cpp_api_dispatch_key(int key, int mode) {
    if (mode == 1) bgl::keyboard::_dispatch_keydown(key);
    if (mode == 0) bgl::keyboard::_dispatch_keyup(key);
    return API_NOFAILURE;
}
int cpp_api_render() {
    bgl::context::clear(0.0,0.0,1.0,0.0);
    bgl::blendmode::use( BLENDMODE_OVER, []() {
        text_render(0.0f,0.0f,1.0,1.0,1.0,"HELLO WORLD");
    });

    GraphicsTest->render();
    game->stage.resources_quad.view();
    game->stage.buildings_quad.view();

    game->view();
    //hwgfx_render_test();

    return API_NOFAILURE;
}
int cpp_api_dispatch_text(char* text) {
    return API_NOFAILURE;
}
int cpp_api_set_screensize(int w, int h) {
    return API_NOFAILURE;
}
int cpp_api_map_keycode(char* code_definition) {
    return API_NOFAILURE;
}
