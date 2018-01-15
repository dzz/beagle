#ifndef LOGGING_CAMP_H
#define LOGGING_CAMP_H
#include "../../quadtree.h"
#include "../../tick_jobs.h"
#include "../resources/resource.h"

class LoggingCamp: public StageResource, public Tickable, public Renderable {
    public:
        LoggingCamp();
        LoggingCamp(float x, float y);
        bool tick();
        void view();
        void set_pos(float x, float y) {
            box.x = x;
            box.y = y;
        }
        Box box = {0};
};

#endif
