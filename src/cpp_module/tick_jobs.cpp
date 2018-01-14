#include "tick_jobs.h"
#include <cstdio>
#include <algorithm>
#include <omp.h>
bool SimpleTick::tick() {
    puts("simple tick\n");
    return true;
}


Job::Job(int job_type): job_type(job_type), static_tickables(), purging_tickables(), renderables() {
    printf("%s job created\n", job_type == JOB_PARALLEL ? "parallel":"sequential");
};
Job::Job(int job_type, int delay): job_type(job_type), delay(delay), static_tickables(), purging_tickables() {
    printf("%s job created at delay %d\n", job_type == JOB_PARALLEL ? "parallel":"sequential", delay);
};

Job::~Job(){
    //cleanup
}

bool Job::tick() {
    puts("ticking job\n");
    if (job_type == JOB_SEQUENTIAL) {
        for (int i = count; i < static_tickables.size(); i += delay) {
            (void)static_tickables[i]->tick();
        }
        for (int i = count; i < purging_tickables.size(); i += delay) {
            purging_tickables[i]->is_finished = purging_tickables[i]->tick();
        }
    } else {//JOB_PARALLEL
#pragma omp parallel
        {
#pragma omp for nowait
            for (int i = count; i < static_tickables.size(); i += delay) {
                (void)static_tickables[i]->tick();
            }
#pragma omp for nowait
            for (int i = count; i < purging_tickables.size(); i += delay) {
                purging_tickables[i]->is_finished = purging_tickables[i]->tick();
            }
        }
    }
    std::remove_if(purging_tickables.begin(),
            purging_tickables.end(),
            [] (Tickable * j) {return j->is_finished;});
    ++count;
    count %= delay;
    return true;
}

void Job::view() {
    puts("viewing job\n");
    if (job_type == JOB_SEQUENTIAL) {
        for (int i = 0; i < renderables.size(); ++i) {
            renderables[i]->view();
        }
    } else {//JOB_PARALLEL
#pragma omp parallel
        {
#pragma omp for
            for (int i = 0; i < renderables.size(); ++i) {
                renderables[i]->view();
            }
        }
    }
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
void Job::add_view_job(Renderable * renderable) {
    puts("adding view\n");
    renderables.push_back(renderable);
}
void Job::remove_view_job(Renderable * renderable) {
    puts("remove view\n");
    std::remove(renderables.begin(), renderables.end(), renderable);
}
