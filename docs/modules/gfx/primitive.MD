#primitive

```python
from client.gfx.primitive import primitive
from client.gfx.primitive import draw_mode 
```

A primitive is a collection of vertexes, which can be two, three or four dimensional. These primitives are stored on the graphics server for efficient rendering. 

A primitive can be created with, or without UV coordinates.


#draw mode

```python
class draw_mode(IntEnum):
    POINTS          = 0 
    LINE_STRIP      = 1 
    LINE_LOOP       = 2 
    LINES           = 3 
    TRI_STRIP       = 4 
    TRI_FAN         = 5
    TRIS            = 6
```

The draw mode specifies how the graphics server should render a primitive

#construction

##vertices only
```python
basic_primitive = primitive( draw_mode.TRI_FAN,
        [
           [  -1.0,  -1.0 ],
           [  1.0,   -0.0,],
           [  1.0,    1.0,],
           [  -1.0,   1.0 ]
         ] 
        )
```

##vertices and UV coords
```python
test_primitive = primitive( primitive_draw_mode.TRI_FAN,
        [
            [ -1.0,    -1.0 ],
            [  1.0,    -1.0,],
            [  1.0,     1.0,],
            [ -1.0,     1.0 ]
        ] ,
         [
            [  0.0,     0.0 ],
            [  0.0,     1.0 ],
            [  1.0,     1.0 ],
            [  1.0,     0.0 ]
         ]
        )
```
#usage

```python
from client.gfx.primitive        import primitive
import client.gfx.shaders        as shaders
from client.gfx.primitive        import draw_mode 

#during or after init()
test_primitive = primitive( draw_mode.TRI_FAN,
        [
           [  -1.0,  -1.0 ],
           [  1.0,   -0.0,],
           [  1.0,    1.0,],
           [  -1.0,   1.0 ]
         ] 
        )
    
#render
shader = shaders.get_client_program( "quad_vertex", "quad_fragment" )
shader.bind([("color", [1.0,0.0,1.0,1.0])])
test_primitive.render()

```



