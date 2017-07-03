import physics
from .shape import shape
from .body import body
from Beagle import API as BGL

class space:
    def __init__(self):
        self._space = physics.space_create()
        self.shapes = []
        self.bodies = []
        self.timestep = 1.0 / BGL.engine.timing.virtual_fps 

    def __del__(self):
        physics.space_drop(self._space)

    def add_fixed_segment(self, p1, p2, friction = 1.0 ):
        segment = shape( physics.fixed_segment_create( self._space, p1[0],p1[1],p2[0],p2[1], friction ))
        self.shapes.append(segment)
        return segment
 
    def add_circular_body(self, p, radius = 1.0, mass = 1.0, friction = 1.0 ):
        (raw_body, raw_shape) = physics.create_circle_body( self._space, radius, mass, p[0], p[1], friction )

        new_body = body(raw_body,raw_shape)
        new_body.update_from_backend()
        self.bodies.append(new_body)
        return new_body

    def await_result(self):
        physics.wait()
        for body in self.bodies:
            body.update_from_backend()

    def tick(self, timestep_divisions = 100.0, solver_iterations = 10.0 ):

        timestep_divisions = 10;
        for body in self.bodies:
            body.synch_to_backend()

        for x in range(0, int(timestep_divisions)):
            physics.space_step(self._space, self.timestep*(1.0/timestep_divisions), solver_iterations )


