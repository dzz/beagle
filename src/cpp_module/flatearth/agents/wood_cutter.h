#ifndef WOOD_CUTTER_H
#define WOOD_CUTTER_H
#include "../stage.h"
#include "../../quadtree.h"
#include "../../tick_jobs.h"
#include "../resources/resource.h"

class WoodCutter: public Stageable, public StageResource, public Tickable, public Renderable, public Quadable {
    public:
        WoodCutter(Stage & stage);
        bool tick();
        void view();
        float r = 1.0, g = 0.0, b = 0.0;
        void set_home(float x, float y) {
            home_x = x;
            home_y = y;
        }
    private:
        float home_x = 0.0, home_y = 0.0;
        float speed = 0.01;
        bool gathering = true;
};

#endif
