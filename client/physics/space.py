import physics
from .shape import shape
from .body import body

class space:
    def __init__(self):
        self._space = physics.space_create()
        self.shapes = []
        self.bodies = []

    def __del__(self):
        physics.space_drop(self._space)

    def add_fixed_segment(self, p1, p2, friction = 1.0 ):
        segment = shape( physics.fixed_segment_create( self._space, p1[0],p1[1],p2[0],p2[1], friction ))
        self.shapes.append(segment)
        return segment
 
    def add_circular_body(self, p, radius = 1.0, mass = 1.0, friction = 1.0 ):
        (raw_body, raw_shape) = physics.create_circle_body( self._space, radius, mass, p[0], p[1], friction )

        self.bodies.append(body(raw_body, raw_shape))
        return

    def tick(self):
        return
