from client.physics.space import space

class FloorPhysics():
    def __init__(self):
        self.physics_space = space()
        self.physics_objects = []
        for occ in self.get_occluders():
            self.physics_space.add_fixed_segment( occ[0], occ[1], self.physics["wall_friction"] )

        for obj in self.objects:
            if(obj.physics):
                obj.body = self.physics_space.add_circular_body( obj.p, obj.physics["radius"], obj.physics["mass"], obj.physics["friction"])
  
    def tick(self):
        for obj in self.objects:
            if(obj.physics):
                obj.body.p[0] = obj.p[0] 
                obj.body.p[1] = obj.p[1] 
                obj.body.v[0] = obj.v[0] 
                obj.body.v[1] = obj.v[1] 

        self.physics_space.tick()
        for obj in self.objects:
            if(obj.physics):
                obj.p[0] = obj.body.p[0]
                obj.p[1] = obj.body.p[1]
                obj.v[0] = obj.body.v[0]
                obj.v[1] = obj.body.v[1]