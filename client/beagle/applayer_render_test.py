import gc
from .hwgfx_test import hwgfx_test
from client.gfx.pointlight import lightmapper
from .test_guppies import test_guppies
from Beagle import API as bgl
from math import sin,cos

class testCamera:
    view = [ 0.1,0.1]
    def translate_position(p):
        return [0.0,0.0]
    def get_uniform_scale():
        return 1.0


class t:
    val = 0.0

def init():
    test_guppies.init()
    pass

def tick():
    t.val += 0.01
    pass

def test_lightmapper():
    encoded_lines = [
		-0.25,-0.25,0.25,-0.25,
		-0.25,0.25,0.25,0.25,
		-0.25,-0.2,-0.25,0.2,
		0.25,-0.2,0.25,0.2
    ]

    lights = [
        {
            "position" : [sin(t.val),cos(t.val)],
            "color" : [ 1.0,1.0,0.0],
            "radius" : 5.0,
        },
        {
            "position" : [cos(t.val),sin(t.val)],
            "color" : [ 0.0,0.0,1.0],
            "radius" : sin(t.val)+3.0,
        },
    ]

    lightmapper.render_scene( encoded_lines, lights, testCamera, None )

def render():
    #hwgfx_test()

    test_lightmapper()
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
