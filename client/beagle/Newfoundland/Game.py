from Beagle import API as BGL
from .Player import Player
from .Controllers import Controllers
from .Camera import Camera

class Game( BGL.simple_tick_manager, BGL.game ):
    def __init__(self):
        BGL.console.attach(self)
        BGL.simple_tick_manager.__init__(self)

        self.camera = self.create_tickable( Camera( p = [0.0,0.0], zoom = 1.0 ) )
        self.controllers = self.create_tickable( Controllers() )
        self.player = self.create_tickable( Player( controllers = self.controllers ) )

        self.initialize()
        
    def initialize(self):
        pass

    def tick(self):
        BGL.simple_tick_manager.tick(self)
        self.camera.p = self.player.p

