#ifndef __WM_HANDLER__
#define __WM_HANDLER__

#include <SDL.h>

// this is for handling os specific WM events.
//
// since the only WM event we should care about is
// tablet packets, this is defined in tablet.c

void handle_wm_event(SDL_Event event); 

#endif
