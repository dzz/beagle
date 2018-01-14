#include <cstdlib>
#include "stage.h"
#include "../quadtree.h"
#include "../tick_jobs.h"
#include "./buildings/logging_camp.h"
#include "./resources/tree.h"

extern "C" unsigned int host_get_screen_width();

extern "C" unsigned int host_get_screen_height();

Stage::Stage():
        resources_quad({0,0,(float)host_get_screen_width(), (float)host_get_screen_height()}),
        buildings_quad({0,0,(float)host_get_screen_width(), (float)host_get_screen_height()}),
        master_job(JOB_SEQUENTIAL),
        tree_job(JOB_SEQUENTIAL),
        logging_camp_job(JOB_SEQUENTIAL) {
    
    master_job.add_static_tick_job(&tree_job);
    master_job.add_view_job(&tree_job);

    master_job.add_static_tick_job(&logging_camp_job);
    master_job.add_view_job(&logging_camp_job);
}

void Stage::init_test_data() {
    for (int i = 0; i < 50;  ++i) {
        Tree * tree = new Tree(100);
        tree->set_pos(rand() % host_get_screen_width(), rand() & host_get_screen_height());
        tree_job.add_purging_tick_job(tree);
        tree_job.add_view_job(tree);
    }
    for (int i = 0; i < 5; ++i) {
        LoggingCamp * camp = new LoggingCamp();
        camp->set_pos(rand() % host_get_screen_width(), rand() & host_get_screen_height());
        logging_camp_job.add_static_tick_job(camp);
        logging_camp_job.add_view_job(camp);
    }
}
