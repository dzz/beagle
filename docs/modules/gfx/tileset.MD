# tileset

```python
    from client.gfx.tileset import tileset
```

A tileset object represents a grid of square images, with optional margin and spacing between each image. Tilesets are useful for rendering sprites and tiles. The tileset format is intended to be compatible with [Tiled](http://www.mapeditor.org/) 

###gid

Each tile has a unique **gid**. Tilemaps can use multiple tilesets, with each tileset handling a range of gids.

##Creating a tileset

```python
    from client.gfx.tileset import tileset

    configuration = {
            "image"         : "roguelikeSheet_transparent.png",
            "imageheight"   : 526,
            "imagewidth"    : 968,
            "margin"        : 0,
            "spacing"       : 1,
            "properties"    : {},
            "firstgid"      : 1,
            "tileheight"    : 16,
            "tileproperties" : {} 
            }

    ts = tileset( configuration, img_path = "roguetiles/Spritesheet/", filtered=True )
```
The second parameter is the path to the tilemap images. Tiled likes to save files with image paths relative to the Tiled project file, but you may want to marshal your assets inside your application directory. This overrides the Tiled default assigned path. Investigate the directory structure of the tilemap_demo application for a pracitcal demonstration of this.

The filtered parameter operates the same as on texture object (ultimately, tilemaps are textures as well) - if True, the tileset will be smoothed if it is scaled up. If false, it will 'pixellate' as the scale increases.

##tile properties

Tiled allows you to define custom sets of properties on a per-tile basis. These might be annotations such as "has_trap" : true for a given class of tile. Note that these are tile wide properties, and do not apply to a specific *placement* of a tile on a map. You can query these with the tile_prop method.

```python
    ## earlier: ts = tileset( configuration, "roguetiles/Spritesheet/" )

    # each of these is equivalent
    is_wall = ts.tile_prop( gid = 731, key = "is_wall" )
    is_wall = ts.tile_prop( 731, "is_wall" )
```


##Drawing a Tile (by gid)

```python
from client.gfx.rect    import rect_tile

#ts is a tilesetobject
#gid is a valid gid

x = 0
y = 0
scale = 2

rect_tile(ts, gid, x, y, scale )
```


