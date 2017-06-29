from .GeometryUtils import ray_segment_intersection, vsub, vmul, smul, vnormal
from math import hypot

class FloorObjectWallCollisions():

    def resolve_object_wall_collision(self, obj):

        cur_p = obj.p


        size = 2.5
        for occ in self.get_occluders():
            last_p = obj.snapshot['p']
            intersection = ray_segment_intersection( [ last_p, cur_p ], occ )
            if intersection:
                ray_dir = intersection[2][1]
                dot = intersection[3]
                length = vsub(intersection, last_p)
                if hypot(length[0],length[1]) < size:
                    print("RAYDIR",ray_dir)
                    print("DOT",dot)

                    safe_p = [0.0,0.0]
                    safe_p[0] = intersection[0] - (intersection[2][1][0]*size)
                    safe_p[1] = intersection[1] - (intersection[2][1][1]*size)

                    obj.p[0] = cur_p[0]*0.5 + safe_p[0]*0.5
                    obj.p[1] = cur_p[1]*0.5 + safe_p[1]*0.5
                    cur_p = obj.p
                    last_p = safe_p
             
    def tick(self):
        for obj in list(filter(lambda x: x.collides_with_walls, self.objects)):
            self.resolve_object_wall_collision( obj )
