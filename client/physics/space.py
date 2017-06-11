import physics

class space:
    def __init__(self):
        self._space = physics.space_create()
    def __del__(self):
        physics.space_drop(self._space)


