from math import atan2
from client.beagle.beagle_api import api as BGL
from .Object import Object

class Player(Object):
    def __init__(self, **kwargs):
        overrides = {
                "num" : 0,
                "speed" : 0.2,
                "controllers" : None,
                "dir" : [ 0.0, 1.0 ],
                "texture" : BGL.assets.get("NL-placeholder/texture/player"),
                "camera" : None,
                'tick_type' : Object.TickTypes.TICK_FOREVER,
                "sight_radius" : 100.0,
                "size": [1.0,1.0],
                "r": 0.75,
            }
        overrides.update(kwargs)
        Object.__init__(self,**overrides)

    def setFloor(self, floor):
        Object.setFloor(self, floor)

    def tick(self):
        pad = self.controllers.get_virtualized_pad( self.num )

        delta = [(pad.left_stick[0])*self.speed,(pad.left_stick[1])*self.speed]
        self.p[0] += delta[0]
        self.p[1] += delta[1]

        self.dir = ( pad.right_stick[0], pad.right_stick[1] )
        self.rad = atan2( self.dir[1], self.dir[0] )
