import physics

class shape():
    def __init__(self, _shape):
        self._shape = _shape

    def __del__(self):
        physics.shape_drop(self._shape)
