from client.beagle.beagle_api import api as BGL

from .Object import Object

class FloorObjectTickManager():
    """ Meta tick manager which routes Objects to an appropriate type of tick manager
    """
    def __init__(self):
        self.simple_tick_manager = BGL.simple_tick_manager()
        self.purging_tick_manager = BGL.purging_tick_manager()

    def tick(self):
        self.simple_tick_manager.tick()
        self.purging_tick_manager.tick()

    def create_tickable(self, obj):
        if(obj.tick_type == Object.TickTypes.STATIC):
            pass
        if(obj.tick_type == Object.TickTypes.TICK_FOREVER):
            self.simple_tick_manager.create_tickable(obj)
        if(obj.tick_type == Object.TickTypes.PURGING):
            self.purging_tick_manager.create_tickable(obj)
