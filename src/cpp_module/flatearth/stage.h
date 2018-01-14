#ifndef STAGE_H
#define STAGE_H
#include "../quadtree.h"
#include "../tick_jobs.h"

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
 
        Quadtree resources_quad;
        Quadtree buildings_quad;

        Job master_job;
        Job tree_job;
        Job logging_camp_job;
};

#endif
