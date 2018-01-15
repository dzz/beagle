#ifndef LOGGING_CAMP_H
#define LOGGING_CAMP_H
#include "../stage.h"
#include "../../quadtree.h"
#include "../../tick_jobs.h"
#include "../resources/resource.h"

class LoggingCamp: public Stageable, public StageResource, public Tickable, public Renderable {
    public:
        LoggingCamp(Stage & stage);
        bool tick();
        void view();
        void set_pos(float x, float y) {
            box.x = x;
            box.y = y;
        }
        Box box = {0};
};

#endif
