from .GeometryUtils import ray_segment_intersection, vsub
from math import hypot

class FloorObjectWallCollisions():

    def resolve_object_wall_collision(self, obj):

        last_p = obj.snapshot['p']
        cur_p = obj.p

        for occ in self.get_occluders():
            intersection = ray_segment_intersection( [ last_p, cur_p ], occ )
            obj.debug_p = intersection
            if intersection:
                print("ISECT",intersection)
                length = vsub(intersection, last_p)
                if hypot(length[0],length[1]) < 2.0:
                    obj.p[0] = last_p[0]
                    obj.p[1] = last_p[1]
             
    def tick(self):
        for obj in list(filter(lambda x: x.collides_with_walls, self.objects)):
            self.resolve_object_wall_collision( obj )
