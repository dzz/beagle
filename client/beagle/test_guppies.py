from Newfoundland.Object import GuppyRenderer
from Beagle import API as bgl

from random import uniform,choice
from math import sin,cos

class fake_camera():
    view = bgl.view.widescreen_16_9

class fake_guppy():
    u_time = 0.0

    def __init__(self):
        self.offs = uniform(0.0,1.0)
        self.p = [ 0.0,0.0]
        self.z_index = self.offs
        self.texture = choice(test_guppies.textures)

    def should_draw(self):
        return True

    def get_camera(self):
        return fake_camera

    def get_shader_params(self):

        fake_guppy.u_time += 0.0005

        u_time = fake_guppy.u_time + self.offs
        return {
            "texBuffer" : self.texture,
            "scale_local" : [ 1.0*(1.0+self.offs),1.0*(1.0-self.offs)],
            "scale_world" : [ sin(u_time), cos(u_time*0.2) ],
            "translation_local" : [ sin(u_time*0.5),cos(u_time*0.25)],
            "translation_world" : [ self.offs,0.0],
            "rotation_local" : sin(u_time*1.2) * 3.14 ,
            "filter_color" : [1.0,1.0,1.0,self.offs],
            "flash_color" : [ 1.0,1.0,1.0,0.0 ]
        }

class test_guppies():
    view = bgl.view.widescreen_16_9
    gr = GuppyRenderer()
    textures = []

    def init():
        for x in range(0,16):
            data = []
            for x in range(0,2*2):
                v = choice([0.0,1.0])
                data.extend([v]*3)
                data.append(choice([0.0,0.5,1.0]))

            print("making fake texture")
            test_guppies.textures.append(bgl.texture.from_data( 2, 2, data ))

    def render():
        guppies = []
        for x in range(0,100):
            guppies.append( fake_guppy() )

        with bgl.blendmode.alpha_over:
            test_guppies.gr.renderObjects(guppies)

        
