from math import sqrt

def rectangles_intersect(a,b):
    if a[0] < b[2] and a[2] > b[0] and a[1] < b[3] and a[3] > b[1]:
        return True

def distance_squared(x1,y1,x2,y2):
    dx = (x2-x1)
    dy = (y2-y1)
    return (dx*dx+dy*dy)

def distance(x1,y1,x2,y2):
    dx = (x2-x1)
    dy = (y2-y1)
    return sqrt(dx*dx+dy*dy)



neighbor_coordinates = [ (-1,  0),
                         (-1, -1),
                         ( 0, -1),
                         ( 1, -1),
                         ( 1,  0),
                         ( 1,  1),
                         ( 0,  1),
                         ( -1, 1) ]

tesselated_unit_quad = [
                         [-1.0, -1.0],
                         [1.0, -1.0],
                         [1.0, 1.0],
                         
                         [1.0, 1.0],
                         [-1.0, 1.0],
                         [-1.0, -1.0],
                        ] 

tesselated_unit_quad_uv = [
                         [0.0, 0.0],
                         [1.0, 0.0],
                         [1.0, 1.0],
                         
                         [1.0, 1.0],
                         [0.0, 1.0],
                         [0.0, 0.0],
                        ] 

def lerp_vec( a, b, x ):
    r = []
    for i in range(0,len(a)):
        r.append(a[i]*(1.0-x))
    for i in range(0,len(r)):
        r[i] = r[i] + (b[i]*x)
    return r

