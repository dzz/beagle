from Beagle import API as BGL

from itertools import chain


class LineTransformer():
    BufferSize = 512
    shader = BGL.assets.get("beagle-nl/shader/transform_geometry")

    def __init__(self):
        self.output_texture = BGL.texture.from_data( LineTransformer.BufferSize, 1, [0.0]*LineTransformer.BufferSize )
        self.output_texture.debugger_attach("LT-output")
        self.output_buffer = BGL.framebuffer.from_texture(self.output_texture)

    def set_encoded_geometry( self, geometry ):
        self.input_texture = BGL.texture.from_data( LineTransformer.BufferSize, 1, geometry )
        self.input_texture.debugger_attach("LT-input")

    def compute(self, params ):
        params["texBuffer"] = self.input_texture
        with BGL.context.render_target( self.output_buffer):
            self.input_texture.render_processed( LineTransformer.shader, params )

    def get_transformed_as_texture(self):
        return self.output_texture
    
class LightMapper(BGL.auto_configurable):
    """ Renders a lightmap to a unit square 
    """

    primitive = BGL.primitive.unit_uv_square

    def __init__(self, **kwargs):

        BGL.auto_configurable.__init__(self,{
            'geometry' : [ [[-1.0,-1.0],[1.0,1.0]], [[1.0,-1.0],[-1.0,1.0]] ],
            'camera' : None,
            'debug_texture_name' : None
        }, **kwargs );

        self.shader = BGL.assets.get("beagle-nl/shader/lightmap")
        self.t = 0.0

        encoded_geometry = LightMapper.encode_geometry( self.geometry ) 
        self.encoded_geometry = encoded_geometry
        self.shader.bind( { "geometry"  : [ encoded_geometry ], 
                            "num_p"     : [ len(encoded_geometry) ],
                            "num_lines" : [ len(self.geometry) ]
                          })

        self.target_tex = BGL.texture.from_data(int(kwargs['width']), int(kwargs['height']),[0.0]*int(kwargs['width'])*int(kwargs['height']*4) )
        #self.target_buffer = BGL.framebuffer.from_dims(  int(kwargs['width']), int(kwargs['height']), filtered = True )
        self.target_buffer = BGL.framebuffer.from_texture( self.target_tex )
        if( self.debug_texture_name ):
            self.target_buffer.get_texture().debugger_attach( self.debug_texture_name )
        self.transformer_computer = LineTransformer()
        self.transformer_computer.set_encoded_geometry( encoded_geometry )

    def set_lights( self, lights ):
        self.lights = lights

    def encode_geometry( geometry ):
        return list(chain(*chain(*geometry)))

    def get_lightmap_texture(self):
        return self.target_buffer.get_texture()

    def get_lightmap_framebuffer(self):
        return self.target_buffer

    def clear(self):
        with BGL.context.render_target( self.target_buffer ):
            BGL.context.clear(0.0,0.0,0.0,1.0)

    def render_lights(self):
       for light in self.lights:
          LightMapper.primitive.render_shaded( self.shader, 
              { 
                  "transformed_lines"   : self.transformer_computer.get_transformed_as_texture(),
                  "position"          : light["position"],
                  "light_color"       : light["color"],
                  "light_radius"      : light["radius"],
                  "view"              : self.camera.view,
                  "translation_world" : self.camera.translate_position([0.0,0.0]),
                  "scale_world"       : self.camera.get_scale()
              })


    def compute(self, clear = True, light_blendmode = BGL.blendmode.add ):
        self.transformer_computer.compute({
            "view"              : self.camera.view,
            "translation_world" : self.camera.translate_position([0.0,0.0]),
            "scale_world"       : self.camera.get_scale()
        })

        with BGL.context.render_target( self.target_buffer ):
            if clear:
                BGL.context.clear(0.0,0.0,0.0,1.0)
            with light_blendmode:
                self.render_lights()

