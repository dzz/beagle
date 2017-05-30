from math import atan2
from client.beagle.beagle_api import api as BGL
from .Object import Object
from .Collider import Collider

class Player(Object, Collider):
    def __init__(self, **kwargs):
        overrides = {
                "num" : 0,
                "speed" : 0.2,
                "controllers" : None,
                "dir" : [ 0.0, 1.0 ],
                "texture" : BGL.assets.get("CE-placeholder-art/texture/player"),
                "camera" : None,
                'tick_type' : Object.TickTypes.TICK_FOREVER,
                "sight_radius" : 20.0,
                "size": [0.75, 1.5],
                "r": 0.75
            }
        overrides.update(kwargs)
        Collider.__init__(self, **overrides)
        Object.__init__(self,**overrides)

        self.delta = [0.0,0.0]

    def get_camera(self):
        return self.camera


    def setFloor(self, floor):
        Object.setFloor(self, floor)
        floor.pebble.subscribe(self, [Collider.Tags.PLAYER], [Collider.Tags.ENEMY])

    def tick(self):
        if self.have_collision():
            #we hit it, for now just move back
            self.p[0] -= self.delta[0]
            self.p[1] -= self.delta[1]
            #dont need to do anything with the actual collision so just clear it
            self.clear_collision()
        pad = self.controllers.get_virtualized_pad( self.num )

        self.delta = [(pad.left_stick[0])*self.speed,(pad.left_stick[1])*self.speed]
        self.p[0] += self.delta[0]
        self.p[1] += self.delta[1]

        self.dir = ( pad.right_stick[0], pad.right_stick[1] )
        self.rad = atan2( self.dir[1], self.dir[0] )
