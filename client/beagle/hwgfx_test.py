import client.gfx.primitive as primitive
import client.gfx.shaders as shaders
from random import uniform

u_time = 0.0

def hwgfx_test():
    global u_time

    test_primitive = primitive.primitive( primitive.draw_mode.TRIS,
    [
        (0.5, -0.5),
        (-0.5, -0.5),
        (-0.5,0.5)
    ],
    [
        (0.0,0.0),
        (1.0,0.0),
        (1.0,1.0)
    ])

    test_shader = shaders.shader( "test/vert", "test/pixel", "shaders/" )
    test_shader.bind({"u_time": u_time})
    u_time = u_time + 0.01;
    test_primitive.render_shaded( test_shader )
    return
