#include "tree.h"
#include "resource.h"

#include "../../../hwgfx/blend_control.h"
#include "../../../hwgfx/text.h"
#include "../../cpp_api/hwgfx/cpp_blendmode.h"
#include "../../cpp_api/hwgfx/cpp_context.h"

Tree::Tree(): StageResource(), Tickable(), Renderable() {

}

Tree::Tree(double amount): StageResource(), Tickable(), Renderable() {
    set_resource(WOOD, amount);
}


bool Tree::tick() {
    puts("im a tree!\n");
    //sit in the sun? grow?
    return true;
}

void Tree::view() {
    bgl::blendmode::use( BLENDMODE_OVER, [this]() {
        text_render(box.x,box.y,0.1,1.0,0.0,"TREE");
    });
}
