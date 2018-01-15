#ifndef TREE_H
#define TREE_H
#include "../../quadtree.h"
#include "../../tick_jobs.h"
#include "resource.h"

class Tree: public StageResource, public Tickable, public Renderable {
    public:
        Tree();
        Tree(double amount);
        bool tick();
        void view();
        void set_pos(float x, float y) {
            box.x = x;
            box.y = y;
        }
        Box box = {0};
};

#endif
