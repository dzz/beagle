
#include "../memory.h"
#include <SDL.h>
#include <SDL_gamecontroller.h>

#include "gamepad.h"
#include "../system/rt_module_codes.h"
#include "../system/log.h"

#include <stdio.h>

hw_gamepad Gamepads[MAX_PADS];
hw_gamepad* MappedPads[MAX_PADS];

int _dirty = 0;

int GamepadDequeueIsDirty() {
    if(_dirty=1) {
        _dirty=0;
        return 1;
    }
    return 0;
}

void resetGamepad() {
    int i;

    for(i=0; i<MAX_PADS; ++i) {
        Gamepads[i].available = 0;
        Gamepads[i].controller = 0;
        Gamepads[i].left_x = 0;
        Gamepads[i].left_y = 0;
        Gamepads[i].right_x = 0;
        Gamepads[i].right_y = 0;
        MappedPads[i] = 0;
    }
}

void GamepadHandleEvent( SDL_Event* event) {

    if( event->type == SDL_JOYAXISMOTION ) {
        hw_gamepad* gp = MappedPads[ event->jaxis.which ];
        _dirty = 1;
        if(gp) {
            switch( event->jaxis.axis ) {
                case 0:
                    gp->left_x = (double)event->jaxis.value / 32276.0;
                    break;
                case 1:
                    gp->left_y = (double)event->jaxis.value / 32276.0;
                    break;
                case 2:
                    gp->right_x = (double)event->jaxis.value / 32276.0;
                    break;
                case 3:
                    gp->right_y = (double)event->jaxis.value / 32276.0;
                    break;
                case 4:
                    gp->left_trigger = (double)event->jaxis.value / 32276.0;
                    break;
                case 5:
                    gp->right_trigger = (double)event->jaxis.value / 32276.0;
                    break;
            }
        //   printf(" gp(%-20f,%-20f,%-20f,%-20f)\n",gp->left_x,gp->left_y, gp->right_x, gp->right_y );
        }
    }
}

hw_gamepad* getGamepad(int index) {
    return( &Gamepads[index] );
}

unsigned int gamepad_get_button(hw_gamepad* gp, unsigned int button) {
    if (gp->available) {
        return (unsigned int)SDL_GameControllerGetButton(gp->controller, button);
    } return 0;
}

unsigned int initGamepad() {

    int found = 0;
    int i;

    if( SDL_InitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER ) < 0)
        return MODULE_FAILURE;

    resetGamepad();

    log_message(CTT2_RT_MODULE_GAMEPAD, LOG_LEVEL_INFO, "I see %i potential controllers.",SDL_NumJoysticks());
    for(i=0; i<SDL_NumJoysticks(); ++i) {
        if(SDL_IsGameController(i)) {
            hw_gamepad *gp = &Gamepads[found]; 
            found++;
            gp->controller = SDL_GameControllerOpen(i);
            gp->index = i;
            gp->available = 1;
            MappedPads[i] = gp;
        }
    }

    log_message(CTT2_RT_MODULE_GAMEPAD, LOG_LEVEL_INFO, "initialized %i gamepads",found);
    return MODULE_LOADED;
}

void dropGamepad() {
    int i;

    for(i=0; i<MAX_PADS; ++i) {

        hw_gamepad *gp = &Gamepads[i];

        if(gp->available) {
            log_message(CTT2_RT_MODULE_GAMEPAD, LOG_LEVEL_INFO, "closing gamepad %i", gp->index);
            SDL_GameControllerClose( gp->controller );
        }
    }
}
