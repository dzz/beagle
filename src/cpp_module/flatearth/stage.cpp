#include <cstdlib>
#include <utility>
#include <random>
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
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> wid_dis{1, (int)host_get_screen_width()};
    std::uniform_int_distribution<int> hei_dis{1, (int)host_get_screen_height()};
    for (int i = 0; i < 1024;  ++i) {
        create_object<Tree>(TREE, (float)wid_dis(gen), (float)hei_dis(gen), 100);
    }
    for (int i = 0; i < 3; ++i) {
        //create_object<LoggingCamp>(LOGGING_CAMP, (float)wid_dis(gen), (float)hei_dis(gen));
    }
}
template<class T, typename... Args> T* Stage::create_object(StageType type, float x, float y, Args... args) {
    T * temp = new T(*this, std::forward<Args>(args)...);
    switch(type) {
        case TREE:
            temp->set_pos(x, y);
            tree_job.add_purging_tick_job(temp);
            tree_job.add_view_job(temp);
            resources_quad.add_box(&(temp->box), temp);
        case LOGGING_CAMP:
            temp->set_pos(x, y);
            logging_camp_job.add_static_tick_job(temp);
            logging_camp_job.add_view_job(temp);
            buildings_quad.add_box(&(temp->box), temp);
    }
    return temp;
}
