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

class Renderable {
    public:
        virtual void view() = 0;
};

//just for usage demo
class SimpleTick: public Tickable {
    public:
        bool tick();
};

class Job: public Tickable, public Renderable {
    public:
        //initializing
        Job(int job_type);
        Job(int job_type, int delay);
        ~Job();

        //nesting
        bool tick();
        void view();

        //ticking
        void add_static_tick_job(Tickable * tickable);
        void add_purging_tick_job(Tickable * tickable);
        void remove_static_tick_job(Tickable * tickable);
        void remove_purging_tick_job(Tickable * tickable);
        //rendering
        void add_view_job(Renderable * renderable);
        void remove_view_job(Renderable * renderable);

        //debug stats
        int total_job_count() {
            return static_tickables.size() + purging_tickables.size() + renderables.size();
        }
        int total_static_job_count() {
            return static_tickables.size();
        }
        int total_purging_job_count() {
            return purging_tickables.size();
        }
        int total_renderable_job_count() {
            return renderables.size();
        }
    private:
        int job_type;
        int count = 0;
        int delay = 1;
        std::vector<Tickable *> static_tickables;
        std::vector<Tickable *> purging_tickables;

        std::vector<Renderable *> renderables;
};

#endif
