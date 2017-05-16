#ifndef __CTT2_HOST__
#define __CTT2_HOST__

#define API_FAILURE 1
#define API_NOFAILURE 0

int api_init();
int api_tick();
int api_drop();
int api_dispatch_mouseup(int button, int x, int y);
int api_dispatch_mousedown(int button, int x, int y);
int api_dispatch_mousemotion(int x, int y);
int api_dispatch_key(int key, int mode);
int api_render();
int api_dispatch_text(char* text);
int api_set_screensize(int w, int h);
int api_map_keycode(char* code_definition);

#endif
