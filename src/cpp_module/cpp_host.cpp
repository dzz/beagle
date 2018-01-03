#include "cpp_host.h"
#include <stdio.h>

int cpp_api_init() {
    puts("init\n");
    return API_NOFAILURE;
}
int cpp_api_tick() {
    puts("tick\n");
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
