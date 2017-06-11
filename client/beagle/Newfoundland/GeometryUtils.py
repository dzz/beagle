from math import hypot

def side(a,b,c):
    """ Returns a position of the point c relative to the line going through a and b
        Points a, b are expected to be different
    """
    d = (c[1]-a[1])*(b[0]-a[0]) - (b[1]-a[1])*(c[0]-a[0])
    return 1 if d > 0 else (-1 if d < 0 else 0)

def is_point_in_closed_segment(a, b, c):
    """ Returns True if c is inside closed segment, False otherwise.
        a, b, c are expected to be collinear
    """
    if a[0] < b[0]:
        return a[0] <= c[0] and c[0] <= b[0]
    if b[0] < a[0]:
        return b[0] <= c[0] and c[0] <= a[0]

    if a[1] < b[1]:
        return a[1] <= c[1] and c[1] <= b[1]
    if b[1] < a[1]:
        return b[1] <= c[1] and c[1] <= a[1]

    return a[0] == c[0] and a[1] == c[1]

#
def closed_segment_intersect(a,b,c,d):
    """ Verifies if closed segments a, b, c, d do intersect.
    """
    if a == b:
        return a == c or a == d
    if c == d:
        return c == a or c == b

    s1 = side(a,b,c)
    s2 = side(a,b,d)

    # All points are collinear
    if s1 == 0 and s2 == 0:
        return \
            is_point_in_closed_segment(a, b, c) or is_point_in_closed_segment(a, b, d) or \
            is_point_in_closed_segment(c, d, a) or is_point_in_closed_segment(c, d, b)

    # No touching and on the same side
    if s1 and s1 == s2:
        return False

    s1 = side(c,d,a)
    s2 = side(c,d,b)

    # No touching and on the same side
    if s1 and s1 == s2:
        return False

    return True

def line_intersection(line1, line2):
    xdiff = (line1[0][0] - line1[1][0], line2[0][0] - line2[1][0])
    ydiff = (line1[0][1] - line1[1][1], line2[0][1] - line2[1][1]) #Typo was here

    def det(a, b):
        return a[0] * b[1] - a[1] * b[0]

    div = det(xdiff, ydiff)
    if div == 0:
       return None

    d = (det(*line1), det(*line2))
    x = det(d, xdiff) / div
    y = det(d, ydiff) / div

    return [ x, y, d ]

def segments_intersect(line1,line2):
    def intersect(line1, line2):
        A,B,C,D = line1[0],line1[1],line2[0],line2[1]
        return closed_segment_intersect(A,B,C,D)
        ##def ccw(A,B,C):
        ##    return (C[1]-A[1]) * (B[0]-A[0]) > (B[1]-A[1]) * (C[0]-A[0])
        ##return ccw(A,C,D) != ccw(B,C,D) and ccw(A,B,C) != ccw(A,B,D)
    val = intersect(line1,line2)
    return val

def line_segment_intersection_position(line1,line2):
    if segments_intersect(line1,line2):
        return line_intersection(line1,line2)
    else:
        return None





def vmul(v1,v2):
    return [ v1[0] * v2[0], v1[1] * v2[1] ]

def smul(v1,v2):
    return (v1[0]*v2[0]) + (v1[1]*v2[1])

def scross(v1,v2):
    return ( v1[0] * v2[1]) - (v1[1] * v2[0])

def vsub(v1,v2):
    return [ v1[0]-v2[0], v1[1]-v2[1] ]

def vnormal(v1):
    l = hypot( v1[0], v1[1])
    if( l == 0.0 ):
        return [0.0,0.0]

    return [ v1[0]/l, v1[1]/l ] 

def ray_segment_intersection( base_ray, seg ):

    #convert to origin / direction
    ray = [ base_ray[0], [ base_ray[1][0]-base_ray[0][0], base_ray[1][1]-base_ray[0][1] ] ]


    if ray[1][0] + ray[1][1] == 0.0:
        return 

    l = hypot( ray[1][0],ray[1][1])
    ray[1][0] = ray[1][0]/l
    ray[1][1] = ray[1][1]/l


    v1 = vsub( ray[0], seg[0] )
    v2 = vsub( seg[1], seg[0] )
    v3 = [ -1*ray[1][1], ray[1][0] ]
    dot = smul(v2,v3)

    if abs(dot) < 0.000001:
        return None

    t1 = scross(v2,v1) / dot
    t2 = smul(v1,v3) / dot

    if (t1>=0.0 and (t2>=0.0 and t2<=1.0)):
        rl = [ ray[1][0]*t1, ray[1][1]*t1 ]
        isect = [ ray[0][0] + rl[0], ray[0][1] + rl[1], ray, dot ]
        return isect 

    return None


ray = [ [0.0,0.0], [9.0,1.0] ]
seg = [ [5.0,-10.0], [15.0,10.0] ]
print("test ray", ray)
print("test seg", seg)
print(ray_segment_intersection( ray, seg))

