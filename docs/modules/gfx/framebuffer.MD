# framebuffer
 
```python
from client.gfx.framebuffer import *
```

It is commonly required to render something to a texture, in order to do something else later on. For example, rendering your world zoomed out onto a texture so you may later scale that and place it somewhere in your UI, or rendering the view from a security camera to display on a monitor elsewhere.

Framebuffers are the mechanism by which this is accomplished. Rendering to texture is a powerful technique that can be used to create a wide variety of effects, such as feedback, or motion blurs.

You can construct a framebuffer object from a prexisting texture. A context manager is used to bind that framebuffer for rendering operations.

# usage

```python
from    client.gfx.framebuffer import *
from    client.gfx.texture import texture
import  client.gfx.context as gfx_context


minimap_texture         = texture.from_dims(256,256)
minimap_framebuffer     = framebuffer.from_texture( minimap_texture )

with framebuffer_as_render_target( minimap_framebuffer ):
    gfx_context.clear(1.0,0.0,0.0,1.0)                      #clear the texture to red
    
```
