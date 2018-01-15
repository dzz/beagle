#include <algorithm>
#include <cstdio>
#include "logging_camp.h"

#include "../../../hwgfx/text.h"
#include "../../cpp_api/hwgfx/cpp_blendmode.h"
#include "../../cpp_api/hwgfx/cpp_context.h"
#include "../resources/resource.h"
#include "../resources/tree.h"

LoggingCamp::LoggingCamp(Stage & stage):Stageable(stage), StageResource(), Tickable(), Renderable() {

}

bool LoggingCamp::tick() {
    std::vector<std::pair<Box*, void *>> results;
    Box collision_box{box.x - 50, box.y - 50, box.w + 100, box.h + 100};
    stage.resources_quad.get_boxes(results, collision_box);
    printf("grabbed %d boxes at (%f,%f)\n", results.size(), box.x, box.y);
    for(auto& pair : results) {
        Tree * tree = reinterpret_cast<Tree*>(pair.second);
        int total_tree_wood = tree->get_resource(WOOD);
        int change = std::min(1, total_tree_wood);
        set_resource(WOOD, get_resource(WOOD) + change);
        tree->set_resource(WOOD, total_tree_wood - change);
        tree->r = 1.0;
    }
    //get nearby trees and take wood from them
    return true;
}

void LoggingCamp::view() {
    bgl::blendmode::use( BLENDMODE_OVER, [&]() {
        text_render(box.x,box.y, r, g, b,"C");
    });
    Box cbox{box.x - 50, box.y - 50, box.w + 100, box.h + 100};
    float tl_x = cbox.x,          tl_y = cbox.y;
    float tr_x = cbox.x + cbox.w, tr_y = cbox.y;
    float bl_x = cbox.x,          bl_y = cbox.y + cbox.h;
    float br_x = cbox.x + cbox.w, br_y = cbox.y + cbox.h;

    bgl::blendmode::use( BLENDMODE_OVER, [&]() {
        text_render(tl_x-4, tl_y-4, r, g, b, "+");
        text_render(tr_x-4, tr_y-4, r, g, b, "+");
        text_render(bl_x-4, bl_y-4, r, g, b, "+");
        text_render(br_x-4, br_y-4, r, g, b, "+");
    });
}
