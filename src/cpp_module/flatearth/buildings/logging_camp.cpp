#include "logging_camp.h"

#include "../../../hwgfx/blend_control.h"
#include "../../../hwgfx/text.h"
#include "../../cpp_api/hwgfx/cpp_blendmode.h"
#include "../../cpp_api/hwgfx/cpp_context.h"

LoggingCamp::LoggingCamp(Stage & stage):Stageable(stage), StageResource(), Tickable(), Renderable() {

}

bool LoggingCamp::tick() {
    //get nearby trees and take wood from them
    return true;
}

void LoggingCamp::view() {
    bgl::blendmode::use( BLENDMODE_OVER, [this]() {
        text_render(box.x,box.y,1.0,0.0,0.0,"CAMP");
    });
}
