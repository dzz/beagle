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
};

class Job: public Tickable {
    public:
        Job(int job_type);
        Job(int job_type, int delay);
        ~Job();

        bool tick();

        void add_static_tick_job(Tickable * tickable);
        void add_purging_tick_job(Tickable * tickable);
        void remove_static_tick_job(Tickable * tickable);
        void remove_purging_tick_job(Tickable * tickable);
    private:
        int job_type;
        int count = 0;
        int delay = 0;
        std::vector<Tickable *> static_tickables;
        std::vector<Tickable *> purging_tickables;
};

#endif
