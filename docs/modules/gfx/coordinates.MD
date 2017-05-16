#coordinates
```python
import client.gfx.coordinates as coordinates
```

Coordinate systems are used by higher level graphics objects (e.g. ``tilemap``, ``sprite``) to specify the coordinate system for rendering.

#usage
```python
import client.gfx.coordinates as coordinates

#
#create a coordinate system 
#
#         -540 
#        +-----+  
#  -960  |  0  | +960
#        +-----+
#          540

coordinate_system = coordinates.center_view( 1920, 1080 )

#center_view defaults to the y axis inverted, as is typical for graphics
#rendering. However, standard mathematical orientation can be used as well.

coordinate_system = coordinates.center_view( 1920, 1080, coordinates.Y_Axis_Down ) 
coordinate_system = coordinates.center_view( 1920, 1080, coordinates.Y_Axis_Up ) 

#available coordinate systems

```
center_view
```

A view centered on 0,0 extending to -Width/2 to +Width/2 on the X-Axis, and -Height/2 +Height/2 on the Y-Axis



