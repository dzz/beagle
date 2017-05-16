#math.helpers

##rectangles_intersect
```python
from client.math.helpers import rectangles_intersect

rectangles_intersect(a,b) #returns True if A and B intersect
```

This function takes two rectangles in the form ``[x1,y1,x2,y2]`` and returns true if they intersect.

##neighbor_coordinates
```python
from client.math.helpers import neighbor_coordinates
```

It is often useful in tile-based systems to iterate over over the 8 neighbors relative to a given position. ``neighbor_coordinates`` is simply a list of normalized x,y tuples that can be iterated on.

```python
#the actual object:

neighbor_coordinates = [ (-1,  0),
                         (-1, -1),
                         ( 0, -1),
                         ( 1, -1),
                         ( 1,  0),
                         ( 1,  1),
                         ( 0,  1),
                         ( -1, 1) ]


#how it might be used 

def cast_fire_spell( center_x,center_y ):
    for x,y in neighbor_coordinates:
        mygame.set_tile_on_fire( center_x +x, center_y +y )

```

###tesselated_unit_quad
```python
from client.math.helpers import tesselated_unit_quad

# equivalent to:

tesselated_unit_quad = [
                         [-1.0, -1.0],
                         [1.0, -1.0],
                         [1.0, 1.0],
                         
                         [1.0, 1.0],
                         [-1.0, 1.0],
                         [-1.0, 1.0],
                        ] 
```

This object is a tesselated unit quad useful for building primitives It is compatible with the TRIS primitive draw mode.

It can be useful to create primitives with this for testing vertex and fragment shaders.

###distance, distance_squared
```python
from client.math.helpers import distance, distance_squared

def distance_squared(x1,y1,x2,y2):
    dx = (x2-x1)
    dy = (y2-y1)
    return (dx*dx+dy*dy)

def distance(x1,y1,x2,y2):
    dx = (x2-x1)
    dy = (y2-y1)
    return sqrt(dx*dx+dy*dy)
```

