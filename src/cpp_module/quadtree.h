#ifndef QUADTREE_H
#define QUADTREE_H

#include <vector>
#include "tick_jobs.h"

#define MAX_LEVELS 8
#define MAX_ITEMS 16

struct Box {
    int x;
    int y;
    int w;
    int h;
};

class Quadtree {
    public:
        Quadtree(int level, Box box);
        ~Quadtree();
        void add_box(Box * box);
        void get_boxes(std::vector<Box *>& results, Box& collision);
        void get_boxes(std::vector<Box *>& results, int x, int y);
        void add_all(std::vector<Box *>& results);
        const Box& getBox() {return box;}
    private:
        void subdevide();

        int level;
        Box box;
        std::vector<Box *> boxes;
        //tl tr bl br
        Quadtree * quads[4] = {nullptr, nullptr, nullptr, nullptr};
};
#endif
