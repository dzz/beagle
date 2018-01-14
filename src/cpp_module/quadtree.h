#ifndef QUADTREE_H
#define QUADTREE_H

#include <vector>
#include "tick_jobs.h"

#define MAX_LEVELS 8
#define MAX_ITEMS 16

class Box {
    public:
        float x;
        float y;
        float w;
        float h;
};

class Quadtree {
    public:
        Quadtree(Box box);
        ~Quadtree();
        void add_box(Box * box, void * object);
        void get_boxes(std::vector<std::pair<Box *, void *>>& results, Box& collision);
        void get_boxes(std::vector<std::pair<Box *, void *>>& results, int x, int y);
        void add_all(std::vector<std::pair<Box *, void *>>& results);
        const Box& getBox() {return box;}
    private:
        Quadtree(int level, Box box);
        void subdevide();

        int level;
        Box box;
        std::vector<std::pair<Box *, void *>> boxes;
        //tl tr bl br
        Quadtree * quads[4] = {nullptr, nullptr, nullptr, nullptr};
};
#endif
