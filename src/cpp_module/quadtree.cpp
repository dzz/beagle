#include "quadtree.h"
#include <algorithm>

bool box_contains(const Box& box1, const Box& box2) {
    if (box1.x+box1.w < box2.x) {
        return false;
    }
    if (box2.x+box2.w < box1.x) {
        return false;
    }
    if (box1.y+box1.h < box2.y) {
        return false;
    }
    if (box2.y+box2.h < box1.y) {
        return false;
    }
    return true;
}
bool box_contains(const Box& box1, int x, int y) {
    if (box1.x+box1.w < x) {
        return false;
    }
    if (x < box1.x) {
        return false;
    }
    if (box1.y+box1.h < y) {
        return false;
    }
    if (y < box1.y) {
        return false;
    }
    return true;
}

Quadtree::Quadtree(int level, Box box): level(level), box(box), boxes() {

}
Quadtree::~Quadtree() {
    if (*quads) {
        delete quads[0];
        delete quads[1];
        delete quads[2];
        delete quads[3];
    }
}

void Quadtree::subdevide() {
    quads[0] = new Quadtree(level + 1, {box.x, box.y, box.w/2, box.h/2});
    quads[1] = new Quadtree(level + 1, {box.x + box.w/2, box.y, box.w/2, box.h/2});
    quads[2] = new Quadtree(level + 1, {box.x, box.y + box.h/2, box.w/2, box.h/2});
    quads[3] = new Quadtree(level + 1, {box.x + box.w/2, box.y + box.h/2, box.w/2, box.h/2});
    std::remove_if(boxes.begin(), boxes.end(),
            [this] (const auto& pair) {
                Box * box = pair.first;
                if (box_contains(quads[0]->getBox(), *box)) {
                    quads[0]->add_box(pair.first, pair.second);
                    return true;
                } else if (box_contains(quads[1]->getBox(), *box)) {
                    quads[1]->add_box(pair.first, pair.second);
                    return true;
                } else if (box_contains(quads[2]->getBox(), *box)) {
                    quads[2]->add_box(pair.first, pair.second);
                    return true;
                } else if (box_contains(quads[3]->getBox(), *box)) {
                    quads[3]->add_box(pair.first, pair.second);
                    return true;
                } else {
                    return false;
                }
            });
}

void Quadtree::add_box(Box * box, void * object) {
    if (boxes.size() == MAX_ITEMS && !*quads && level != MAX_LEVELS) {
        boxes.push_back(std::make_pair(box, object));
        subdevide();
    } else if (*quads) {
        //if it crashes horribly check to ensure that the whole quadtrees actually exist
        if (box_contains(quads[0]->getBox(), *box)) {
            quads[0]->add_box(box, object);
        } else if (box_contains(quads[1]->getBox(), *box)) {
            quads[1]->add_box(box, object);
        } else if (box_contains(quads[2]->getBox(), *box)) {
            quads[2]->add_box(box, object);
        } else if (box_contains(quads[3]->getBox(), *box)) {
            quads[3]->add_box(box, object);
        } else {
            boxes.push_back(std::make_pair(box, object));
        }
    } else {
        boxes.push_back(std::make_pair(box, object));
    }
}
void Quadtree::add_all(std::vector<std::pair<Box *, void *>>& results) {
    if (*quads) {
        quads[0]->add_all(results);
        quads[1]->add_all(results);
        quads[2]->add_all(results);
        quads[3]->add_all(results);
    }
    results.insert(results.end(), boxes.begin(), boxes.end());
}
void Quadtree::get_boxes(std::vector<std::pair<Box *, void *>>& results, Box& collision) {
    if (!*quads) {
        results.insert(results.end(), boxes.begin(), boxes.end());
    } else {
        if (box_contains(quads[0]->getBox(), collision)) {
            quads[0]->get_boxes(results, collision);
        } else if (box_contains(quads[1]->getBox(), collision)) {
            quads[1]->get_boxes(results, collision);
        } else if (box_contains(quads[2]->getBox(), collision)) {
            quads[2]->get_boxes(results, collision);
        } else if (box_contains(quads[3]->getBox(), collision)) {
            quads[3]->get_boxes(results, collision);
        } else {
            add_all(results);
        }
    }
}
void Quadtree::get_boxes(std::vector<std::pair<Box *,void *>>& results, int x, int y) {
    if (!*quads) {
        results.insert(results.end(), boxes.begin(), boxes.end());
    } else {
        if (box_contains(quads[0]->getBox(), x, y)) {
            quads[0]->get_boxes(results, x, y);
        } else if (box_contains(quads[1]->getBox(), x, y)) {
            quads[1]->get_boxes(results, x, y);
        } else if (box_contains(quads[2]->getBox(), x, y)) {
            quads[2]->get_boxes(results, x, y);
        } else if (box_contains(quads[3]->getBox(), x, y)) {
            quads[3]->get_boxes(results, x, y);
        } else {
            add_all(results);
        }
    }
}
