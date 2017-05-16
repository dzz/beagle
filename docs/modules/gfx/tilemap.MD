# tilemap
```python
from client.gfx.tilemap import tilemap
```
Tilemaps are used to draw a collection of gids. They manage their own tilesets, and can be loaded from files or constructed dynamically. Tilemaps require a ``coordinate system`` for rendering. When a tilemap is compiled, the default tile size for the underlying GPU primitives is based off of the tilsets ``tileheight`` property. This can be overridden if required.

# usage
```python
from client.gfx.tilemap         import tilemap

#from a Tiled json exported file, use default (-1,+1) coordinate system and tileheight

tm = tilemap.from_json_file( "json/mymap.json", img_path = "tileimages/", filtered=False ):

#supply a coordinate system and alternate tilesize

from client.gfx.coordinates     import centered_view, Y_Axis_Down

coord_system = centered_view(1920,1080,Y_Axis_Down)
tm = tilemap.from_json_file( "json/mymap.json", img_path = "tileimages/", filtered=False, coordinates = coord_system, tileheight = 32 ):

#use a configuration dict instead of a json file
configuration = { 
    "tileheight":16,
    "layers":[
    {
        "width":3,
        "height":3,
        "data":[ 1,1,1,1,0,1,1,1,1 ] #these are gids, see tileset
    }, 
    ],
    "tilesets":[    #each item in this array is the same as you would pass to a tileset constructor
    {
        "firstgid":1,
        "image":"mytiles.png",
        "imageheight":526,
        "imagewidth":968,
        "margin":0,
        "name":"roguelikeSheet_transparent",
        "tileproperties": { },
        "properties" : { },
        "spacing":1,
        "tileheight":16,
        "tilewidth":16
    }]
}

tm = tilemap( configuration, img_path, filtered=False )
```

#rendering a tilemap

```python

tm.render( x = int(camera[0]), y = int(camera[1]), scale = 2 )
```

#changing coordinate systems
```python
import client.gfx.coordinates as coordinates

tm.set_coordinate_system( coordinates.center_view(500,500,coordinates.Y_Axis_Down) )
```
