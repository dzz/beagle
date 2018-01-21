#ifndef WOOD_CUTTER_H
#define WOOD_CUTTER_H
#include "../stage.h"
#include "../../quadtree.h"
#include "../../jobs.h"
#include "../resources/resource.h"
#include "../buildings/logging_camp.h"

class LoggingCamp;

class WoodCutter: public Stageable, public StageResource, public Jobable, public Quadable {
    public:
        WoodCutter(Stage & stage, LoggingCamp& camp);
        bool tick();
        void view();
        float r = 1.0, g = 0.0, b = 0.0;
    private:
        LoggingCamp& camp;
        float speed = 0.1;
        bool gathering = true;
};

#endif
