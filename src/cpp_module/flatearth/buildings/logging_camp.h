#ifndef LOGGING_CAMP_H
#define LOGGING_CAMP_H
#include "../stage.h"
#include "../../quadtree.h"
#include "../../jobs.h"
#include "../resources/resource.h"
#include "../agents/wood_cutter.h"

class WoodCutter;
class LoggingCamp: public Stageable, public StageResource, public Jobable, public Quadable {
    public:
        LoggingCamp(Stage & stage);
        bool tick();
        void view();
        
        void init_test_workers();

        float r = 1.0, g = 0.0, b = 0.0;
    private:
        Job worker_job;
        std::vector<WoodCutter> workers;
};

#endif
