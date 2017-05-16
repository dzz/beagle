#ifndef __HW_GAMEPAD__
#define __HW_GAMEPAD__

#include <SDL.h>

#define MAX_PADS 8

typedef struct {
    int index;
    SDL_GameController* controller;
    char available;

    double left_x;
    double left_y;

    double right_x;
    double right_y;

    double left_trigger;
    double right_trigger;

} hw_gamepad;

unsigned int initGamepad();
void dropGamepad();
hw_gamepad* getGamepads();
void GamepadHandleEvent(SDL_Event* event);
hw_gamepad* getGamepad(int index);
unsigned int gamepad_get_button(hw_gamepad* gp, unsigned int button);
int GamepadDequeueIsDirty();


#endif
