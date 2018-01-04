#include "tick_jobs.h"
#include <cstdio>
#include <algorithm>
#include <omp.h>
bool SimpleTick::tick() {
    puts("simple tick\n");
    return true;
}


Job::Job(int job_type): job_type(job_type), static_tickables(), purging_tickables() {
    printf("%s job created\n", job_type == JOB_PARALLEL ? "parallel":"sequential");
};

Job::~Job(){
    //cleanup
}

bool Job::tick() {
    puts("ticking job\n");
    if (job_type == JOB_SEQUENTIAL) {
        for (auto j : static_tickables) {
            (void)j->tick();
        }
        for (auto j : purging_tickables) {
            j->is_finished = j->tick();
        }
    } else {//JOB_PARALLEL
#pragma omp parallel
        {
#pragma omp for nowait
            for (int i = 0; i < static_tickables.size(); ++i) {
                (void)static_tickables[i]->tick();
            }
#pragma omp for nowait
            for (int i = 0; i < purging_tickables.size(); ++i) {
                purging_tickables[i]->is_finished = purging_tickables[i]->tick();
            }
        }
    }
    std::remove_if(purging_tickables.begin(),
            purging_tickables.end(),
            [] (Tickable * j) {return j->is_finished;});
    return true;
}

void Job::add_static_tick_job(Tickable * tickable) {
    puts("adding static\n");
    static_tickables.push_back(tickable);
}
void Job::add_purging_tick_job(Tickable * tickable) {
    puts("adding purging\n");
    purging_tickables.push_back(tickable);
}
void Job::remove_static_tick_job(Tickable * tickable) {
    puts("remove static\n");
    std::remove(static_tickables.begin(), static_tickables.end(), tickable);
}
void Job::remove_purging_tick_job(Tickable * tickable) {
    puts("remove purging\n");
    std::remove(purging_tickables.begin(), purging_tickables.end(), tickable);
}
