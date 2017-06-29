from client.math.helpers import lerp_vec
import client.system.log as log
from math import tanh,pi,sin,cos
class curve:
    def __init__(self,points):
        if len(points)==0:
            raise ValueError("attempted to create animation curve with 0 points")

        mode_vec = False
        mode_str = False
        vec_len = None
        mix_msg = "attempted to create animation curve with mixed vector/string data"
        self.max_t = 0.0
        for point in points:
            self.max_t = max(self.max_t, point["t"])
            if "string" in point:
                if mode_vec:
                    raise ValueError(mix_msg)
                mode_str = True
            if "vec" in point:
                if mode_str:
                    raise ValueError(mix_msg)
                if vec_len is None:
                    vec_len = len(point["vec"])
                elif vec_len is not len(point["vec"]):
                    raise ValueError("attempted to create curve with mixed-length vectors")

        self.points = points

    def value_at(self,t):
        left = None
        right = None
        for point in self.points:
            if point["t"] <t:
                left = point
            if point["t"] >t:
                right = point
                break

        if left and right:
            if "vec" in left:
                nt = (t - left["t"]) / (right["t"]-left["t"])
                if("smooth" in right) and right["smooth"]:
                    if right["smooth"] == "tanh":
                        nt = tanh(nt*pi)
                    if right["smooth"] == "sqr":
                        nt = nt*nt
                    if right["smooth"] == "invsqr":
                        nt = 1- ((1-nt*nt)*(1-nt*nt))
                    if right["smooth"] == "sin":
                        nt = (1.0+(cos(nt * pi)+pi))/2.0
                return lerp_vec(left["vec"],right["vec"],nt)
            if "string" in left:
                return left["string"]

        if left:
            if "vec" in left:
                return left["vec"]
            if "string" in left:
                return left["string"]
            return None

        if right:
            if "vec" in right:
                return right["vec"]
            if "string" in right:
                return right["string"]
            
        if "vec" in points[0]:
            return self.points[0]["vec"]
        if "string" in points[0]:
            return self.points[0]["string"]

