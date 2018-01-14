#ifndef LUMBERYARD_H
#define LUMBERYARD_H
#include "../../quadtree.h"
#include "../../tick_jobs.h"

class Lumberyard: public Tickable, public Renderable {
    public:
        Lumberyard();
        ~Lumberyard();
        bool tick();
        void view();
    private:
        
};
#endif
