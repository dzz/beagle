import gc
from .hwgfx_test import hwgfx_test
from .test_guppies import test_guppies
#from Beagle import API as bgl
#
#class TestState:
#    primitive = bgl.primitive.unit_uv_square
#    shader = bg.assets.get('beagle-2d /shader/beagle-2d')
#     
def init():
    test_guppies.init()
    pass

def tick():
    pass

def render():
    hwgfx_test()
    test_guppies.render()

def finalize():
    pass

def configure(data):
    pass
