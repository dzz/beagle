import client.system.log as log
import client.gfx.texture as texture
from client.gfx.primitive import primitive, draw_mode
from client.math.helpers import tesselated_unit_quad, tesselated_unit_quad_uv
from client.system.video import *
import hwgfx

class framebuffer:
    stack = []
    screen_primitive = primitive( draw_mode.TRIS, tesselated_unit_quad, tesselated_unit_quad_uv )
    def __init__(self, fb, texture):
        self._fb   = fb
        #we grab a reference to the texture so we don't garbage collect and flush
        #the associated texture from the graphics server
        self._tex  = texture 

        log.write( log.DEBUG, "Acquired framebuffer:{0}".format(self._fb))

    def __del__(self):
        hwgfx.framebuffer_drop(self._fb)
        log.write( log.DEBUG, "Dropped framebuffer:{0}".format(self._fb))

    def as_render_target(self):
        return render_target(self)

    def get_texture(self):
        return self._tex

    def bind_as_texture(self,texture_unit = 0 ):
        self._tex.bind(texture_unit)

    def render_shaded( self, shader_program, shader_inputs = {} ):
        return self.render_processed( shader_program, shader_inputs )

    def render_processed( self, shader_program, shader_inputs = [] ):
        """ Renders the Framebuffer with a given shader program and inputs """
        shader_program.bind( shader_inputs, False, 1 ) 
        self.bind_as_texture( texture.units[0] )
        framebuffer.screen_primitive.render()
        
    @classmethod
    def from_texture(cls,texture):
        fb = hwgfx.framebuffer_create()
        hwgfx.framebuffer_bind_texture( fb, texture._tex )
        return cls(fb,texture)

    @classmethod
    def from_screen(cls, filtered = False ):
       tex = texture.texture.from_dims(get_screen_width(),get_screen_height(),filtered)
       return framebuffer.from_texture(tex)

    @classmethod
    def from_dims(cls,x,y, filtered = False ):
        tex = texture.texture.from_dims(x,y, filtered)
        return framebuffer.from_texture(tex)

class framebuffer_as_render_target:
    def __init__(self,framebuffer):
        self.framebuffer = framebuffer

    def __enter__(self):
        framebuffer.stack.append(self.framebuffer)
        hwgfx.framebuffer_render_start( self.framebuffer._fb )
        hwgfx.viewport_set(0,0,self.framebuffer._tex.w,self.framebuffer._tex.h)

    def __exit__(self, exc_type, exc_value, traceback):
        if(len(framebuffer.stack)>0):
            framebuffer.stack.pop()
            if(len(framebuffer.stack)>0):
                hwgfx.framebuffer_render_start( framebuffer.stack[-1]._fb )
                hwgfx.viewport_set(0,0,framebuffer.stack[-1]._tex.w,self.framebuffer._tex.h)
            else:
                hwgfx.framebuffer_render_end( self.framebuffer._fb )
                hwgfx.viewport_reset()

class render_target(framebuffer_as_render_target):
    pass
