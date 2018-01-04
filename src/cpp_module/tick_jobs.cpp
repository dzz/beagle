#include "tick_jobs.h"
#include <cstdio>
#include <algorithm>
#include <omp.h>
bool SimpleTick::tick() {
    puts("simple tick\n");
    return true;
}


Job::Job(int job_type): job_type(job_type), static_jobs(), purging_jobs() {
    printf("%s job created\n", job_type == JOB_PARALLEL ? "parallel":"sequential");
};

Job::~Job(){
    //cleanup
}

bool Job::tick() {
    puts("ticking job\n");
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
            for (int i = 0; i < static_jobs.size(); ++i) {
                (void)static_jobs[i]->tick();
            }
#pragma omp for nowait
            for (int i = 0; i < purging_jobs.size(); ++i) {
                purging_jobs[i]->is_finished = purging_jobs[i]->tick();
            }
        }
    }
    std::remove_if(purging_jobs.begin(),
            purging_jobs.end(),
            [] (Tickable * j) {return j->is_finished;});
    return true;
}

void Job::add_static_tick_job(Tickable * job) {
    puts("adding static\n");
    static_jobs.push_back(job);
}
void Job::add_purging_tick_job(Tickable * job) {
    puts("adding purging\n");
    purging_jobs.push_back(job);
}
void Job::remove_static_tick_job(Tickable * job) {
    puts("remove static\n");
    std::remove(static_jobs.begin(), static_jobs.end(), job);
}
void Job::remove_purging_tick_job(Tickable * job) {
    puts("remove purging\n");
    std::remove(purging_jobs.begin(), purging_jobs.end(), job);
}
