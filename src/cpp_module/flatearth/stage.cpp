#include <cstdlib>
#include <utility>
#include <random>
#include <algorithm>
#include "stage.h"
#include "../quadtree.h"
#include "../jobs.h"
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
    
    master_job.add_static(&tree_job);
    master_job.add_static(&logging_camp_job);
}

void Stage::init_test_data() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> wid_dis{1, (int)host_get_screen_width()};
    std::uniform_int_distribution<int> hei_dis{1, (int)host_get_screen_height()};
    for (int i = 0; i < 256;  ++i) {
        create_object<Tree>(TREE, (float)wid_dis(gen), (float)hei_dis(gen), 100);
    }
    for (int i = 0; i < 3; ++i) {
        LoggingCamp * temp = create_object<LoggingCamp>(LOGGING_CAMP, (float)wid_dis(gen), (float)hei_dis(gen));
        temp->init_test_workers();
    }
}
template<class T, typename... Args> T* Stage::create_object(StageType type, float x, float y, Args... args) {
    T * temp = new T(*this, std::forward<Args>(args)...);
    switch(type) {
        case TREE:
            temp->set_pos(x, y);
            tree_job.add_purging(temp);
            resources_quad.add_box(temp);
            break;
        case LOGGING_CAMP:
            temp->set_pos(x, y);
            logging_camp_job.add_static(temp);
            buildings_quad.add_box(temp);
            break;
    }
    all_objects.push_back(temp);
    return temp;
}

void Stage::cleanup() {
    for (auto& pair : pending_destroy) {
            all_objects.erase(std::remove(all_objects.begin(), all_objects.end(), pair.second), all_objects.end());
        switch(pair.first) {
            case TREE:
                {
                    Tree * temp = static_cast<Tree*>(pair.second);
                    temp->node->remove(temp);
                    delete temp;
                    pair.second = (Stageable *)0xDEAD;
                }
                break;
            case LOGGING_CAMP:
                {
                    LoggingCamp * temp = static_cast<LoggingCamp*>(pair.second);
                    temp->node->remove(temp);
                    delete temp;
                    pair.second = (Stageable *)0xDEAD;
                }
                break;
        }
    }
    pending_destroy.clear();
}
