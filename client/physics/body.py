import gc
import physics

class body:
    def __init__(self, raw_body, raw_shape, space):
        self._body = raw_body;
        self._shape = raw_shape;
        self.space = space
        self.p = [ 0.0, 0.0 ]
        self.v = [ 0.0, 0.0 ]

    def __del__(self):
        print("BODY DELETE", self, "IN SPACE", self.space)

        if(self.space.destroyed):
            print("... couldn't delete it. This is maybe acceptable, on exit, only.")
            return

        if self._shape:
            physics.shape_drop(self._shape)
        if self._body:
             physics.body_drop(self._body)
        self.space = None
        self._body = None
        self._shape = None

    def synch_to_backend(self):
        physics.body_set_state( self._body, self.p[0],self.p[1],self.v[0],self.v[1])

    def update_from_backend(self):
        data = physics.body_get_state(self._body)
        self.p[0] = data[0]
        self.p[1] = data[1]
        self.v[0] = data[2]
        self.v[1] = data[3]

