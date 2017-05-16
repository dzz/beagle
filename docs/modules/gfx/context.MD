# context

```python
import client.gfx.context as gfx_context
```

The graphical context module is reserved for operations which change the global state of the GPU but do not bind to a particular 'object'. Currently it is only used for setting the global clear color, and clearing the currently active framebuffer. 

#usage

```python

import client.gfx.context as gfx_context

#Set RGBA values for the clear color. This represents a state change on the graphics card.

gfx_context.set_clear_color(0.0,1.0,1.0,1.0)

#Clear the screen 

gfx_context.clear()
```
