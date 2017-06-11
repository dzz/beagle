import physics
import .shape

class space:
    def __init__(self):
        self._space = physics.space_create()
        self.shapes = []

    def __del__(self):
        physics.space_drop(self._space)

    def add_fixed_segment(self, p1, p2, friction = 1.0 ):
        self.shapes.append( physics.fixed_segment_create( self._space, p1[0],p1[1],p2[0],p2[1], friction ))
    
