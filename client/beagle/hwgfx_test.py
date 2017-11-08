import client.gfx.primitive as primitive
import client.gfx.shaders as shaders

def hwgfx_test():

    test_primitive = primitive.primitive( primitive.draw_mode.TRIS,
    [
        (1.0, 1.0),
        (-1.0, 1.0),
        (-1.0,-1.0)
    ],
    [
        (0.0,0.0),
        (1.0,0.0),
        (1.0,1.0)
    ])

    test_shader = shaders.shader( "test/vert", "test/pixel", "shaders/" )
    test_primitive.render_shaded( test_shader )
    return
