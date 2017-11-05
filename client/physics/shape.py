import physics

class shape():
    def __init__(self, _shape):
        self._shape = _shape
        self.destroyed = False

    def destroy(self):
        if(self.destroyed):
            raise Exception("Tried to destroy a physics shape which has already been destroyed")
        if self._shape:
            physics.shape_drop(self._shape)
        self._shape = None
        self.destroyed = True

    #def __del__(self):
    #    if not self.destroyed:
    #        pass
