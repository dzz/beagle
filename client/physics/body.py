import physics

class body:
    def __init__(self, raw_body, raw_shape):
        self._body = raw_body;
        self._shape = raw_shape;

    def __del__(self):
        physics.shape_drop(self._shape)
        physics.body_drop(self._body)

