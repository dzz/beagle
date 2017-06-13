import physics

class body:
    def __init__(self, raw_body, raw_shape):
        self._body = raw_body;
        self._shape = raw_shape;
        self.p = [ 0.0, 0.0 ]
        self.v = [ 0.0, 0.0 ]

    def __del__(self):
        physics.shape_drop(self._shape)
        physics.body_drop(self._body)

    def synch_to_backend(self):
        physics.body_set_state( self._body, self.p[0],self.p[1],self.v[0],self.v[1])

    def update_from_backend(self):
        data = physics.body_get_state(self._body)
        self.p[0] = data[0]
        self.p[1] = data[1]
        self.v[0] = data[2]
        self.v[1] = data[3]

