#include <algorithm>
#include <cstdio>
#include "logging_camp.h"

#include "../../../hwgfx/text.h"
#include "../../cpp_api/hwgfx/cpp_blendmode.h"
#include "../../cpp_api/hwgfx/cpp_context.h"
#include "../resources/resource.h"
#include "../resources/tree.h"

LoggingCamp::LoggingCamp(Stage & stage):Stageable(stage), StageResource(), Tickable(), Renderable(), Quadable(), worker_job(JOB_SEQUENTIAL) {
    box = {0,0,10,10};
}

void LoggingCamp::init_test_workers() {
    workers.emplace_back(stage);
    workers.back().set_home(box.x, box.y);
    workers.back().set_pos(box.x, box.y);
    worker_job.add_purging_tick_job(&workers.back());
    worker_job.add_view_job(&workers.back());
}

bool LoggingCamp::tick() {
    worker_job.tick();
    //do stuff with the wood?
    return true;
}

void LoggingCamp::view() {
    worker_job.view();
    
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
