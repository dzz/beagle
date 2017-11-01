import physics

class shape():
    def __init__(self, _shape):
        self._shape = _shape

    def destroy(self):
        if self._shape:
            physics.shape_drop(self._shape)
        self._shape = None

    def __del__(self):
        if self._shape:
            physics.shape_drop(self._shape)
        self._shape = None
