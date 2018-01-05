#include "cpp_host.h"
#include "tick_jobs.h"
#include <cstdio>
#include "../hwgfx/text.h"
#include "../hwgfx/blend_control.h"
#include "./cpp_api/hwgfx/Blendmode.h"

extern "C" {
    void hwgfx_render_test();
}

Job tick_manager(JOB_PARALLEL), nested(JOB_SEQUENTIAL);
SimpleTick st1{}, st2{}, st3{}, st4{};
Tickable * t1, * t2, * t3, * t4;
int cpp_api_init() {
    puts("init\n");
    t1 = &st1;
    t2 = &st2;
    t3 = &st3;
    t4 = &st4;
    nested.add_static_tick_job(t3);
    nested.add_static_tick_job(t4);
    tick_manager.add_static_tick_job(t1);
    tick_manager.add_static_tick_job(t2);
    tick_manager.add_static_tick_job(&nested);
    return API_NOFAILURE;
}
int cpp_api_tick() {
    puts("tick\n");
    tick_manager.tick();
    return API_NOFAILURE;
}
int cpp_api_drop() {
    puts("drop\n");
    return API_NOFAILURE;
}
int cpp_api_dispatch_mouseup(int button, int x, int y) {
    puts("dispatch mouseup\n");
    return API_NOFAILURE;
}
int cpp_api_dispatch_mousedown(int button, int x, int y) {
    puts("dispatch mousedown\n");
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
int cpp_api_render() {
    puts("render\n");

    Blendmode::Use( BLENDMODE_OVER, []() {
        text_render(0.0f,0.0f,1.0,1.0,1.0,"HELLO WORLD");
    });

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
