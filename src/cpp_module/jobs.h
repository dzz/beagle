#ifndef JOBS_H
#define JOBS_H

#include <vector>

#define JOB_PARALLEL 0
#define JOB_SEQUENTIAL 1

class Jobable {
    public:
        virtual bool tick() = 0;
        virtual void view() = 0;
        bool is_finished = false;
};


class Job: public Jobable {
    public:
        //initializing
        Job(int job_type);
        Job(int job_type, int delay);
        ~Job();

        //nesting
        bool tick();
        void view();

        void add_static(Jobable * tickable);
        void add_purging(Jobable * tickable);
        void remove_static(Jobable * tickable);
        void remove_purging(Jobable * tickable);

        //debug stats
        int total_job_count() {
            return statics.size() + purgings.size();
        }
        int total_static_job_count() {
            return statics.size();
        }
        int total_purging_job_count() {
            return purgings.size();
        }
    private:
        int job_type;
        int count = 0;
        int delay = 1;
        std::vector<Jobable *> statics;
        std::vector<Jobable *> purgings;
};

#endif
