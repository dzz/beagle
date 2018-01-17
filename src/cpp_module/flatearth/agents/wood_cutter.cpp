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

WoodCutter::WoodCutter(Stage & stage):Stageable(stage), StageResource(), Tickable(), Renderable(), Quadable() {
    box = {0,0,10,10};
}

void WoodCutter::view() {
    bgl::blendmode::use( BLENDMODE_OVER, [&]() {
        text_render(box.x,box.y, r, g, b,"W");
    });
}

bool WoodCutter::tick() {
    if (gathering) {
        std::vector<Quadable *> results;
        Box collision_box{home_x-20, home_y-20, home_x + 20, home_y + 20};
        stage.resources_quad.get_boxes(results, collision_box);
        float hypot_x, hypot_y,
              hypot_n = 1000.0, hypot_t,
              mov_x = 0, mov_y = 0;
        for(const auto& tree : results) {
            hypot_x = tree->box.x - box.x;
            hypot_y = tree->box.y - box.y;
            if ((hypot_t = hypot(hypot_x, hypot_y)) < hypot_n) {
                hypot_n = hypot_t;
                mov_x = hypot_x;
                mov_y = hypot_y;
            }
        }
        if (mov_x && mov_y) {
            float angle = atan2(mov_x, mov_y);
            box.x += sin(angle) * speed;
            box.y += cos(angle) * speed;
        }
    }
    return true;
}
