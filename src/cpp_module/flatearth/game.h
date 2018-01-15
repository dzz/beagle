#ifndef GAME_H
#define GAME_H
#include "stage.h"


class Game {
    public:
        Game();
        void tick() {
            stage.tick();
        }
        void view() {
            stage.view();
        }

        Stage stage;
};

#endif
