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

class Quadable;
class Quadtree {
    public:
        Quadtree(Box box);
        ~Quadtree();
        void add_box(Quadable * object);
        void get_boxes(std::vector<Quadable *>& results, Box& collision);
        void get_boxes(std::vector<Quadable *>& results, int x, int y);
        void add_all(std::vector<Quadable *>& results);
        void remove(Quadable * object);
        const Box& getBox() {return box;}
        void view();
    private:
        Quadtree(int level, Box box);
        void subdevide();

        int level;
        Box box;
        std::vector<Quadable *> boxes;
        //tl tr bl br
        Quadtree * quads[4] = {nullptr, nullptr, nullptr, nullptr};
};

class Quadable {
    public:
        void set_pos(float x, float y) {
            box.x = x;
            box.y = y;
        }
        Box box;
        Quadtree * node;
};
#endif
