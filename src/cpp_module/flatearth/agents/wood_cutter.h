#ifndef WOOD_CUTTER_H
#define WOOD_CUTTER_H
#include "../stage.h"
#include "../../quadtree.h"
#include "../../tick_jobs.h"
#include "../resources/resource.h"
#include "../buildings/logging_camp.h"

class LoggingCamp;

class WoodCutter: public Stageable, public StageResource, public Tickable, public Renderable, public Quadable {
    public:
        WoodCutter(Stage & stage, LoggingCamp& camp);
        bool tick();
        void view();
        float r = 1.0, g = 0.0, b = 0.0;
    private:
        LoggingCamp& camp;
        float speed = 0.01;
        bool gathering = true;
};

#endif
