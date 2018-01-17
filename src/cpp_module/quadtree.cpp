#include "quadtree.h"
#include <algorithm>
#include <iterator>
#include <cstdio>

//for render test
#include "./../hwgfx/text.h"
#include "./cpp_api/hwgfx/cpp_blendmode.h"
#include "./cpp_api/hwgfx/cpp_context.h"

bool box_contains(const Box& box1, const Box& box2) {
    return ((box1.x < box2.x && box2.x + box2.w < box1.x + box1.w) &&
            (box1.y < box2.y && box2.y + box2.h < box1.y + box1.h));
}

bool box_collides(const Box& box1, const Box& box2) {
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

bool box_collides(const Box& box1, int x, int y) {
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

Quadtree::Quadtree(Box box): level(0), box(box), boxes() {
    puts("created base quadtree\n");
}
Quadtree::Quadtree(int level, Box box): level(level), box(box), boxes() {
    printf("created level %d quadtree\n", level);
}

Quadtree::~Quadtree() {
    if (*quads) {
        delete quads[0];
        delete quads[1];
        delete quads[2];
        delete quads[3];
    }
}

void Quadtree::view() {
    if (*quads) {
        quads[0]->view();
        quads[1]->view();
        quads[2]->view();
        quads[3]->view();
    }
    float tl_x = box.x,         tl_y = box.y;
    float tr_x = box.x + box.w, tr_y = box.y;
    float bl_x = box.x,         bl_y = box.y + box.h;
    float br_x = box.x + box.w, br_y = box.y + box.h;

    bgl::blendmode::use( BLENDMODE_OVER, [&]() {
            text_render(tl_x-4, tl_y-4, 0.0, 0.0, 1.0, "+");
            text_render(tr_x-4, tr_y-4, 0.0, 0.0, 1.0, "+");
            text_render(bl_x-4, bl_y-4, 0.0, 0.0, 1.0, "+");
            text_render(br_x-4, br_y-4, 0.0, 0.0, 1.0, "+");
            });
}

void Quadtree::subdevide() {
    quads[0] = new Quadtree(level + 1, {box.x,           box.y,           box.w/2, box.h/2});
    quads[1] = new Quadtree(level + 1, {box.x + box.w/2, box.y,           box.w/2, box.h/2});
    quads[2] = new Quadtree(level + 1, {box.x,           box.y + box.h/2, box.w/2, box.h/2});
    quads[3] = new Quadtree(level + 1, {box.x + box.w/2, box.y + box.h/2, box.w/2, box.h/2});
    std::remove_if(boxes.begin(), boxes.end(),
            [&] (const auto& object) {
            if (box_contains(quads[0]->getBox(), object->box)) {
            quads[0]->add_box(object);
            return true;
            } else if (box_contains(quads[1]->getBox(), object->box)) {
            quads[1]->add_box(object);
            return true;
            } else if (box_contains(quads[2]->getBox(), object->box)) {
            quads[2]->add_box(object);
            return true;
            } else if (box_contains(quads[3]->getBox(), object->box)) {
            quads[3]->add_box(object);
            return true;
            } else {
            return false;
            }
            });
}

void Quadtree::add_box(Quadable * object) {
    puts("adding box\n");
    if (boxes.size() == MAX_ITEMS && !*quads && level != MAX_LEVELS) {
        boxes.push_back(object);
        object->node = this;
        subdevide();
    } else if (quads[0] != nullptr) {
        if (box_contains(quads[0]->getBox(), object->box)) {
            quads[0]->add_box(object);
        } else if (box_contains(quads[1]->getBox(), object->box)) {
            quads[1]->add_box( object);
        } else if (box_contains(quads[2]->getBox(), object->box)) {
            quads[2]->add_box(object);
        } else if (box_contains(quads[3]->getBox(), object->box)) {
            quads[3]->add_box(object);
        } else {
            boxes.push_back(object);
            object->node = this;
        }
    } else {
        boxes.push_back(object);
        object->node = this;
    }
}
void Quadtree::add_all(std::vector<Quadable *>& results) {
    if (quads[0] != nullptr) {
        quads[0]->add_all(results);
        quads[1]->add_all(results);
        quads[2]->add_all(results);
        quads[3]->add_all(results);
    }
    results.insert(results.end(), boxes.begin(), boxes.end());
}
void Quadtree::get_boxes(std::vector<Quadable *>& results, Box& collision) {
    if (quads[0] != nullptr) {
        if (box_collides(quads[0]->getBox(), collision)) {
            quads[0]->get_boxes(results, collision);
        }
        if (box_collides(quads[1]->getBox(), collision)) {
            quads[1]->get_boxes(results, collision);
        }
        if (box_collides(quads[2]->getBox(), collision)) {
            quads[2]->get_boxes(results, collision);
        }
        if (box_collides(quads[3]->getBox(), collision)) {
            quads[3]->get_boxes(results, collision);
        }
    }
    std::copy_if(boxes.begin(), boxes.end(), std::back_inserter(results), [&](const auto& check) {
            return box_collides(check->box, collision);
            });
}
void Quadtree::get_boxes(std::vector<Quadable *>& results, int x, int y) {
    if (quads[0] != nullptr) {
        if (box_collides(quads[0]->getBox(), x, y)) {
            quads[0]->get_boxes(results, x, y);
        }
        if (box_collides(quads[1]->getBox(), x, y)) {
            quads[1]->get_boxes(results, x, y);
        }
        if (box_collides(quads[2]->getBox(), x, y)) {
            quads[2]->get_boxes(results, x, y);
        }
        if (box_collides(quads[3]->getBox(), x, y)) {
            quads[3]->get_boxes(results, x, y);
        }
    }
    std::copy_if(boxes.begin(), boxes.end(), std::back_inserter(results), [&](const auto& check) {
            return box_collides(check->box, x, y);
            });
}

void Quadtree::remove(Quadable * object) {
    std::remove(boxes.begin(), boxes.end(), object);
}
