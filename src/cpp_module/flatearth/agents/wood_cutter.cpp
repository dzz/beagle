#include "wood_cutter.h"
#include "../buildings/logging_camp.h"
#include "../../../hwgfx/text.h"
#include "../../cpp_api/hwgfx/cpp_blendmode.h"
#include "../../cpp_api/hwgfx/cpp_context.h"
#include "../resources/resource.h"
#include "../resources/tree.h"

#include <cstdio>
#include <algorithm>
#include <cmath>

WoodCutter::WoodCutter(Stage & stage, LoggingCamp& camp):Stageable(stage), StageResource(), Tickable(), Renderable(), Quadable(), camp(camp) {
    box = {0,0,4,4};
}

void WoodCutter::view() {
    bgl::blendmode::use( BLENDMODE_OVER, [&]() {
        text_render(box.x - 4, box.y - 4, r, g, b,"W");
    });
}

bool WoodCutter::tick() {
        float hypot_x, hypot_y,
              hypot_n = -1.0, hypot_t,
              mov_x = 0, mov_y = 0;
        if (gathering) {
            std::vector<Quadable *> results;
            Box collision_box{camp.box.x - 20, camp.box.y - 20, 40, 40};
            stage.resources_quad.get_boxes(results, collision_box);
            for(const auto& tree : results) {
                Tree * tree_ptr = static_cast<Tree *>(tree);
                tree_ptr->r = 1.0;
                hypot_x = tree->box.x - box.x;
                hypot_y = tree->box.y - box.y;
                if ((hypot_t = hypot(hypot_x, hypot_y)) < hypot_n || hypot_n == -1.0) {
                    hypot_n = hypot_t;
                    mov_x = hypot_x;
                    mov_y = hypot_y;
                }
                if (collision::box_collides(tree->box, box)) {
                    float wood = tree_ptr->get_resource(WOOD);
                    tree_ptr->set_resource(WOOD, 0);
                    set_resource(WOOD, wood);
                    gathering = false;
                    break;
                }
            }
            if (mov_x && mov_y) {
                float angle = atan2(mov_x, mov_y);
                box.x += sin(angle) * speed;
                box.y += cos(angle) * speed;
            }
        } else {
            mov_x = camp.box.x - box.x;
            mov_y = camp.box.y - box.y;
            if (mov_x && mov_y) {
                float angle = atan2(mov_x, mov_y);
                box.x += sin(angle) * speed;
                box.y += cos(angle) * speed;
            }
            if (collision::box_collides(box, camp.box)) {
                gathering = true;
            }
        }
    return true;
}
