import hwgfx
import client.system.log as log
from client.gfx.primitive import primitive, draw_mode
from client.math.helpers import tesselated_unit_quad, tesselated_unit_quad_uv

units = range(0,256)
slots = [None] * 256;

class texture:
    texture_lookup = {}
    units = range(0,256)
    screen_primitive = primitive( draw_mode.TRIS, tesselated_unit_quad, tesselated_unit_quad_uv )
    
    def __init__(self, tex, w, h ):
        self._tex   = tex
        self.w      = w
        self.h      = h
        self.debugger_name = None
        #log.write(log.DEBUG, "Acquired texture {0}".format(self._tex))

    def get_width(self):
        return self.w

    def get_height(self):
        return self.h

    def get_texture(self):
        #yup. for interface compatilibity with framebuffers which are useful
        #to use 'like' textures sometimes
        return self

    def debugger_attach(self, name):
        i = 0
        test_name = name
        while test_name in texture.texture_lookup:
            test_name = "{0}({1})".format(name,i)
            i = i+1
            
        texture.texture_lookup[test_name] = self 
        self.debugger_name = test_name

    def __del__(self):
        if(self.debugger_name):
            del texture.texture_lookup[self.debugger_name]
        hwgfx.texture_drop(self._tex)
        #log.write(log.DEBUG, "Dropped texture {0}".format(self._tex))

    @classmethod
    def from_local_image(cls, local_image, filtered=False):
        tex = hwgfx.texture_generate( local_image.w, 
                                local_image.h, 
                                filtered)
        hwgfx.texture_upload(tex, local_image._img)
        #log.write(log.DEBUG, "Generated texture {0} from local image {1}".format(tex, local_image._img))
        return cls( tex, 
                    local_image.w, 
                    local_image.h )

    @classmethod
    def from_data(cls, w,h,data):
        tex = hwgfx.texture_generate_fp( w, h, data)
        return cls(tex,w,h)

    @classmethod
    def from_dims(cls, w,h,filtered=False):
        tex = hwgfx.texture_generate(w,h,filtered)
        return cls(tex, w, h)

    def render_processed( self, shader_program, shader_inputs = [] ):
        shader_program.bind( shader_inputs ) 
        self.bind( 0 )
        texture.screen_primitive.render()

    def upload_local_image(self,local_image):
        hwgfx.texture_upload(self._tex, local_image._img)

    def download_local_image(self, local_image):
        hwgfx.texture_download(self._tex, local_image._img)

    def bind(self, texture_unit):
        hwgfx.texture_bind(self._tex, texture_unit)

