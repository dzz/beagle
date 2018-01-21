#include "tree.h"
#include "resource.h"

#include "../../../hwgfx/text.h"
#include "../../cpp_api/hwgfx/cpp_blendmode.h"
#include "../../cpp_api/hwgfx/cpp_context.h"

Tree::Tree(Stage & stage):Stageable(stage), StageResource(), Jobable() {
    box = {0,0,4,4};
}

Tree::Tree(Stage & stage, double amount):Stageable(stage), StageResource(), Jobable() {
    box = {0,0,4,4};
    set_resource(WOOD, amount);
}


bool Tree::tick() {
    return get_resource(WOOD) > 0;
}

void Tree::view() {
    bgl::blendmode::use( BLENDMODE_OVER, [&]() {
        text_render(box.x - 4, box.y - 4, r, g, b,"T");
    });
}
