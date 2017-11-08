import gc
from .hwgfx_test import hwgfx_test
from .test_guppies import test_guppies
from Beagle import API as bgl
from math import sin,cos

class t:
    val = 0.0

def init():
    test_guppies.init()
    pass

def tick():
    t.val += 0.01
    pass

def render():
    hwgfx_test()
    test_guppies.render()

    fb = bgl.framebuffer.from_dims(320,240)

    with bgl.context.render_target(fb):
        with bgl.blendmode.alpha_over:

            r = (sin(t.val*2)+1.0)/2.
            g = (sin(t.val*1.3)+1.0)/2.
            b = (cos(t.val*0.7)+1.0)/2.

            bgl.lotext.render_text_pixels( "PLEASE RESTART WITH CARTRDIGE INSERTED" , 8.0,240.0-12.0,[r,g,b])

    with bgl.blendmode.alpha_over:
        fb.render_shaded(bgl.assets.get("beagle-2d/shader/passthru"))

def finalize():
    pass

def configure(data):
    pass
