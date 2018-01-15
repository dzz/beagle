#include "tree.h"
#include "resource.h"

#include "../../../hwgfx/blend_control.h"
#include "../../../hwgfx/text.h"
#include "../../cpp_api/hwgfx/cpp_blendmode.h"
#include "../../cpp_api/hwgfx/cpp_context.h"

Tree::Tree(Stage & stage):Stageable(stage), StageResource(), Tickable(), Renderable() {

}

Tree::Tree(Stage & stage, double amount):Stageable(stage), StageResource(), Tickable(), Renderable() {
    set_resource(WOOD, amount);
}


bool Tree::tick() {
    return get_resource(WOOD) > 0;
}

void Tree::view() {
    bgl::blendmode::use( BLENDMODE_OVER, [this]() {
        text_render(box.x,box.y,0.5,1.0,0.0,"TREE");
    });
}
