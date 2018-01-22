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
    if (get_resource(WOOD) > 0) {
        return true;
    } else {
        stage.mark_destroy(TREE, this);
        return false;
    }
}

void Tree::view() {
    bgl::blendmode::use( BLENDMODE_OVER, [&]() {
        text_render(box.x - 4, box.y - 4, r, g, b,"T");
    });
}
