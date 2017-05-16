#shader

```python
import client.gfx.shaders as shaders
from client.gfx.shaders import shader
```

A shader represents a compiled and linked shader program on the graphics server. Shaders are written in the *glsl* language. 

##vertex shader

The vertex shader program converts the input coordinates into clip space, which is a unit box ( extending from -1, +1 on both axis ) representing the entirety of the current ``viewport`` (which may be a different size depending on whether you are rendering to the screen, another texture, etc. )

##fragment shader

After the locations of the vertex have been computed by the graphics server, each pixels final color is computed by the fragment shader. 

#usage

There are two main ways to load shaders. The *shader registry*, or by managing your shader programs manually. The shader registery will lazy load shaders as needed, but will not flush any shaders from memory or the graphics server until the application exits. This is convenient when your application uses a minimal number of shaders. 

The shader registry is accessed by the module function ``get_client_program``

If you require more control over when the shaders get compiled or leave scope, you can create the shader object yourself. This functionality is exposed the class method ``from_application_library`` classmethod on the  ``shader`` object.

Shaders can specify ``uniforms`` which are inputs to the shader program. For example, you may wish to warp the vertexes depending on the state of some property of an object in a game. Uniforms can be single floats, up to 4D vectors.

The ``glsl`` source code for your application should live inside your applications ```shaders/``` folder. You can look at the tilemap_demo for an example.


## loading

```python

#method 1 - import a shader, use the global shader registry
import client.gfx.shaders as shader

my_shader   = shaders.get_client_program( "quad_vertex", "quad_fragment" )  
my_shader2  = shaders.get_client_program( "quad_vertex", "quad_fragment" )  #my_shader2 and my_shader will refer to the same object on the graphics server

exit() # both shaders are cleaned up now


#method 2 - import a shader directly, using classmethods
from client.gfx.shaders import shader

my_shader = shader.from_application_library("quad_vertex", "quad_fragment" )        # quad_vertex/quad_fragment is compiled and loaded

def scoped_load_shader():
    my_shader2 = shader.from_application_library("quad_vertex","quad_fragment" )    # will reload and compile (does not reuse my_shader)
    return                                                                          # my_shader2 will be garbage collected and eventually removed 
                                                                                    # from the graphics server

```

## binding uniforms and rendering

To render using a shader, it must be bound. Only one shader can be bound at a time. After a shader is bound, any rendering will use that configuration.

At the time of binding, uniforms must also be configured. Uniforms are passed in as an array of tuples, with the value they will be set to.

```python

# assume my_shader is a compiled shader object, and it accepts a uniform input "tint_color" which is an RGBA value

my_shader.bind( [ ("tint_color", [1.0,0.5,0.5,1.0] ) ] )

# subsequent render operations (such as primitive.render) will now use this shader. 

```

