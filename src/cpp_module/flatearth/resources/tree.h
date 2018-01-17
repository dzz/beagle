#ifndef TREE_H
#define TREE_H
#include "../stage.h"
#include "../../quadtree.h"
#include "../../tick_jobs.h"
#include "resource.h"

class Tree: public Stageable, public StageResource, public Tickable, public Renderable, public Quadable {
    public:
        Tree(Stage& stage);
        Tree(Stage& stage, double amount);
        bool tick();
        void view();
        float r = 0.0, g = 1.0, b = 0.0;
};

#endif
