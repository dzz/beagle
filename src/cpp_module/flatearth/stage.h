#ifndef STAGE_H
#define STAGE_H
#include "../quadtree.h"
#include "../tick_jobs.h"

class Stage;

class Stageable {
    public:
        Stageable(Stage& stage): stage(stage) {
        }
        Stage & stage;
};

enum StageType {
    TREE,
    LOGGING_CAMP
};

class Stage {
    public:
        Stage();
        void tick() {
            (void)master_job.tick();
        }
        void view() {
            master_job.view();
        }
        void init_test_data();
        template<class T, typename... Args> T* create_object(StageType type, Args... args);
 
        Quadtree resources_quad;
        Quadtree buildings_quad;

        Job master_job;
        Job tree_job;
        Job logging_camp_job;
};

#endif
