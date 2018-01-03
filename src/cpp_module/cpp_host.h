#ifndef CPP_HOST_H
#define CPP_HOST_H

#define API_FAILURE 1
#define API_NOFAILURE 0

#ifdef __cplusplus
extern "C" {
#endif

int cpp_api_init();
int cpp_api_tick();
int cpp_api_drop();
int cpp_api_dispatch_mouseup(int button, int x, int y);
int cpp_api_dispatch_mousedown(int button, int x, int y);
int cpp_api_dispatch_mousemotion(int x, int y);
int cpp_api_dispatch_mousewheel(int y);
int cpp_api_dispatch_key(int key, int mode);
int cpp_api_render();
int cpp_api_dispatch_text(char* text);
int cpp_api_set_screensize(int w, int h);
int cpp_api_map_keycode(char* code_definition);

#ifdef __cplusplus
}
#endif

#endif
