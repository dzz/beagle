#sprite
```python
from client.gfx.sprite import sprite
from client.gfx.sprite import sprite_renderer
```

Sprites are two dimensional images which can be rendered. In order to render a sprite, a ``sprite_renderer`` is required, which can be used to efficiently  render batches of sprites which share the same tileset, and coordinate system.

A sprite contains a dict of 'named_animations', each mapping to a list of tile ids. 


#usage

```python
from client.gfx.coordinates      import centered_view, Y_Axis_Down
from client.gfx.tileset          import tileset
from client.gfx.sprite           import sprite, sprite_renderer



#in/after init

coord_system = centered_view(1920,1080,Y_Axis_Down)

#Load a tileset via a configuration dict
configuration = {
        "image"         : "spritesheet.png",
        "imageheight"   : 526,
        "imagewidth"    : 968,
        "margin"        : 0,
        "spacing"       : 1,
        "properties"    : {},
        "firstgid"      : 1,
        "tileheight"    : 16,
        "tileproperties" : {} 
        }

sprite_tileset = tileset( configuration, "roguetiles/Spritesheet/", filtered=True )

#construct a renderer with our desired tileset and coordinate system
renderer = sprite_renderer( tileset = sprite_tileset, coordinates = coord_system )

my_sprite = sprite( sprite_renderer = renderer,                                    #the renderer to bind to 
                 named_animations = { "walk" : [1, 2, 3 ], "run" : [4,5,6] },   #animations
                 current_animation="walk",                                      #starting animation
                 ticks_per_frame = 2,                                           #ticks per frame (default 2)
                 size = 16 )                                                    #logical size of sprite (defaults to tileheight property)


#during render

#use the sprite_renderer object to render a batch of sprite. 
renderer.render( [ [ my_sprite, [ 10.0, -10.0 ], 16 ],    #render my_sprite at 10, -10 at a 16x 
                   [ my_sprite, [  0.0,   0.0 ], 2  ] ] )

```
#more complex transformations:

the ``sprite_renderer`` accepts a list of sprite render operations in the basic form: ``[ sprite, [x,y], scale]``; but sometimes more control is required; in which case you can supply render operations in the form: ``[ sprite, [local_x,local_y], local_scale, [world_x,world_y],world_scale ]``

#animation

In order to animate a sprite you have to call the ``tick`` function on the sprite object. 
You can change the animation with the ``select_animation`` method. Changing the animation will reset the animation cycle, so you only want to issue a select_animation call during a state change, i.e. do not issue a select_animation call on every frame, since that will result in only the first frame of animation showing.

You can disable/enable a sprite from animating by using the method ``set_animation_enabled``

```python
#during tick

my_sprite.tick() 


#changing animation:

if player.WasRunning is False and isRunningNow:
    my_sprite.select_animation("run")
elif player.WasRunning is True and not isRunningNow:
    my_sprite.select_animation("walk")


```


