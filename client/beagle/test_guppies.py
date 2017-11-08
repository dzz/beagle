from Newfoundland.Object import GuppyRenderer
from Beagle import API as bgl

from random import uniform,choice

class fake_camera():
    view = bgl.view.widescreen_16_9

class fake_guppy():

    def __init__(self):
        self.p = [ 0.0,0.0]
        self.z_index = choice([1,2,3])
        self.texture = choice(test_guppies.textures)

    def should_draw(self):
        return True

    def get_camera(self):
        return fake_camera

    def get_shader_params(self):
        return {
            "texBuffer" : self.texture,
            "scale_local" : [ 1.0,1.0],
            "scale_world" : [ 1.0,1.0],
            "translation_local" : [ 0.0,0.0],
            "translation_world" : [ 0.0,0.0],
            "rotation_local" : uniform(-3.14,3.14),
            "filter_color" : [1.0,1.0,1.0,1.0],
        }

class test_guppies():
    view = bgl.view.widescreen_16_9
    gr = GuppyRenderer()
    textures = []

    def init():
        for x in range(0,3):
            data = []
            for x in range(0,64*64):
                data.append(uniform(0.0,1.0))
                data.append(uniform(0.0,1.0))
                data.append(uniform(0.0,1.0))
                data.append(1.0)

            print("making fake texture")
            test_guppies.textures.append(bgl.texture.from_data( 64, 64, data ))

    def render():
        guppies = []
        for x in range(0,100):
            guppies.append( fake_guppy() )

        test_guppies.gr.renderObjects(guppies)

        
