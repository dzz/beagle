#include "jobs.h"
#include <cstdio>
#include <algorithm>
#include <omp.h>

Job::Job(int job_type): job_type(job_type), statics(), purgings() {
    printf("%s job created\n", job_type == JOB_PARALLEL ? "parallel":"sequential");
};
Job::Job(int job_type, int delay): job_type(job_type), delay(delay), statics(), purgings() {
    printf("%s job created at delay %d\n", job_type == JOB_PARALLEL ? "parallel":"sequential", delay);
};

Job::~Job(){
    //cleanup
}

bool Job::tick() {
    if (job_type == JOB_SEQUENTIAL) {
        for (int i = count; i < statics.size(); i += delay) {
            (void)statics[i]->tick();
        }
        for (int i = count; i < purgings.size(); i += delay) {
            purgings[i]->is_finished = purgings[i]->tick();
        }
    } else {//JOB_PARALLEL
#pragma omp parallel
        {
#pragma omp for nowait
            for (int i = count; i < statics.size(); i += delay) {
                (void)statics[i]->tick();
            }
#pragma omp for nowait
            for (int i = count; i < purgings.size(); i += delay) {
                purgings[i]->is_finished = purgings[i]->tick();
            }
        }
    }
    std::remove_if(purgings.begin(),
            purgings.end(),
            [] (Jobable * j) {return j->is_finished;});
    ++count;
    count %= delay;
    return true;
}

void Job::view() {
    if (job_type == JOB_SEQUENTIAL) {
        for (int i = 0; i < statics.size(); ++i) {
            statics[i]->view();
        }
        for (int i = 0; i < purgings.size(); ++i) {
            purgings[i]->view();
        }
    } else {//JOB_PARALLEL
#pragma omp parallel
        {
#pragma omp for
            for (int i = 0; i < statics.size(); ++i) {
                statics[i]->view();
            }
#pragma omp for
            for (int i = 0; i < purgings.size(); ++i) {
                purgings[i]->view();
            }
        }
    }
}

void Job::add_static(Jobable * tickable) {
    puts("adding static\n");
    statics.push_back(tickable);
}
void Job::add_purging(Jobable * tickable) {
    puts("adding purging\n");
    purgings.push_back(tickable);
}
void Job::remove_static(Jobable * tickable) {
    puts("remove static\n");
    std::remove(statics.begin(), statics.end(), tickable);
}
void Job::remove_purging(Jobable * tickable) {
    puts("remove purging\n");
    std::remove(purgings.begin(), purgings.end(), tickable);
}
