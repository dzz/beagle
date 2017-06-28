from client.system.video import *
import beagle_runtime

class beagle_engine():
    class profiler():
        draw_calls = 0
        start_tick = 0.0
        end_tick = 0.0
        start_render = 0.0
        end_render = 0.0
        end_tick = 0.0

        total_frame_time = 0.0
        total_tick_time = 0.0
        total_render_time = 0.0
        total_spin_time = 0.0

        def status_string():
            return "Tick:{0:.0f} - Render:{1:.0f} - Spin:{2:.0f} - Incl:{3:.0f}  - Draw Calls:{4}".format( beagle_engine.profiler.total_tick_time,
                beagle_engine.profiler.total_render_time, beagle_engine.profiler.total_spin_time, beagle_engine.profiler.total_frame_time, beagle_engine.profiler.draw_calls )

    class client():
        path = ''
    class window():
        width = get_screen_width()
        height = get_screen_height()
    class timing():
        virtual_fps = beagle_runtime.get_vfps()
        def get_hf_timer():
            return beagle_runtime.get_hf_timer()

