from client.system.video import *
import beagle_runtime

class beagle_engine():
    class client():
        path = ''
    class window():
        width = get_screen_width()
        height = get_screen_height()
    class timing():
        virtual_fps = beagle_runtime.get_vfps()


