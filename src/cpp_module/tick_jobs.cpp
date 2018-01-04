#include "tick_jobs.h"
#include <cstdio>
#include <algorithm>
#include <omp.h>

Job::Job(int job_type): job_type(job_type), static_jobs(), purging_jobs() {
    printf("%s job created\n", job_type == JOB_PARALLEL ? "parallel":"sequential");
};

bool Job::tick() {
    if (job_type == JOB_SEQUENTIAL) {
        for (auto j : static_jobs) {
            (void)j->tick();
        }
        for (auto j : purging_jobs) {
            j->is_finished = j->tick();
        }
    } else {//JOB_PARALLEL
#pragma omp parallel
        {
#pragma omp for nowait
            for (auto j = static_jobs.begin(); j != static_jobs.end(); ++j) {
                (void)(*j)->tick();
            }
#pragma omp for nowait
            for (auto j = purging_jobs.begin(); j != purging_jobs.end(); ++j) {
                (*j)->is_finished = (*j)->tick();
            }
        }
    }
    std::remove_if(purging_jobs.begin(),
            purging_jobs.end(),
            [] (Tickable * j) {return j->is_finished;});
    return true;
}

void add_static_tick_job(Tickable * job) {
}
void add_purging_tick_job(Tickable * job) {
}
void remove_static_tick_job(Tickable * job) {
}
void remove_purging_tick_job(Tickable * job) {
}
