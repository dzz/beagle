#ifndef TICK_JOBS_H
#define TICK_JOBS_H

#include <vector>

#define JOB_PARALLEL 0
#define JOB_SEQUENTIAL 1

class Tickable {
    public:
        virtual bool tick() = 0;
        bool is_finished = false;
};

class SimpleTick: public Tickable {
    public:
        bool tick();
    private:
        int id;
};

class Job: public Tickable {
    public:
        Job(int job_type);
        ~Job();

        bool tick();

        void add_static_tick_job(Tickable * job);
        void add_purging_tick_job(Tickable * job);
        void remove_static_tick_job(Tickable * job);
        void remove_purging_tick_job(Tickable * job);
    private:
        int job_type;
        std::vector<Tickable *> static_jobs;
        std::vector<Tickable *> purging_jobs;
};

#endif
