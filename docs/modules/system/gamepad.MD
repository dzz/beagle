# gamepad

```python
from client.system.gamepad       import get_gamepad
```
The gamepad module provides a function which you can use anywhere inside your application's tick() function to address a gamepad by its player id. 

# usage

```python

from client.system.gamepad       import get_gamepad

def tick():
    global camera
    pad = get_gamepad(0)
    camera[0] += pad.leftStick[0]*32
    camera[1] += pad.leftStick[1]*32

```

# warnings

currently only have leftStick and righStick. Button bindings will be added on demand.
