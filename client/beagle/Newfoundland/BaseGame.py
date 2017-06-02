from Beagle import API as BGL
from .Player import Player
from .Controllers import Controllers
from .Camera import Camera

class BaseGame( BGL.simple_tick_manager, BGL.game ):
    def __init__(self):
        BGL.console.attach(self)
        BGL.simple_tick_manager.__init__(self)
        self.initialize()
        
    def initialize(self):
        pass

