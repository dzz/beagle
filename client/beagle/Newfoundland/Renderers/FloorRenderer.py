from Beagle import API as BGL
from client.gfx.coordinates import centered_view,Y_Axis_Up

from ..Camera import Camera
from ..Object import Object
from .Lightmapper import Lightmapper

class FloorRenderer(BGL.auto_configurable):
    """ Renderer class for composing with Floor 

        Attributes:
            pixel_copy_shader: static instance to the shader used for copying the compositing stages
    """
    pixel_copy_shader = BGL.assets.get("beagle-2d/shader/passthru")
    compositor_shader = BGL.assets.get("beagle-nl/shader/floor-compositor")

    def __init__(self, **kwargs ):
        BGL.auto_configurable.__init__(self, {
            "static_lightmap_width" : 2048,
            "static_lightmap_height" : 2048,
            "dynamic_lightmap_width" : 128,
            "dynamic_lightmap_height" :128,
            "vision_lightmap_width" : 256,
            "vision_lightmap_height" : 256
        });

        ## used to place composited elements
        self.primitive = BGL.primitive.unit_uv_square
        self.shader = BGL.assets.get("beagle-2d/shader/beagle-2d")

        self.static_lightmap = self.compute_static_lightmap()
        self.dynamic_lightmap = self.configure_dynamic_lightmapper()
        self.vision_lightmap = self.configure_vision_lightmapper()

        self.floor_buffer = BGL.framebuffer.from_screen()
        self.light_buffer = BGL.framebuffer.from_screen()
        self.object_buffer = BGL.framebuffer.from_screen()
        self.height_buffer = BGL.framebuffer.from_screen()
        self.reflect_buffer = BGL.framebuffer.from_screen()
        self.vision_buffer = BGL.framebuffer.from_screen()

    def precompute_frame(self):
        """ Pre-render compositing """

        self.compute_vision_lightmap()
        self.compute_dynamic_lightmap()

        with BGL.context.render_target( self.floor_buffer ):
            self.render_tiles()
        with BGL.context.render_target( self.height_buffer ):
            self.render_tiles("height")
        with BGL.context.render_target( self.reflect_buffer ):
            self.render_tiles("reflection")



        with BGL.context.render_target( self.light_buffer ):
            BGL.context.clear(0.0,0.0,0.0,1.0)
            with BGL.blendmode.add:
                self.render_static_lightmap()
                self.dynamic_lightmap.get_lightmap_framebuffer().render_processed( FloorRenderer.pixel_copy_shader )

        with BGL.context.render_target( self.object_buffer ):
            BGL.context.clear(0.0,0.0,0.0,0.0)
            with BGL.blendmode.alpha_over:
                self.render_objects()
            self.player.render() 

    def render_objects(self):
        """ Render floor objects """
        for obj in self.objects:
            obj.render()

    def render(self):
        """ Perform final composite to active target """
        self.precompute_frame()
        BGL.compositor.render_composite( FloorRenderer.compositor_shader, {
            "camera_position" : self.camera.p,
            "floor_buffer" : self.floor_buffer,
            "light_buffer" : self.light_buffer,
            "object_buffer" : self.object_buffer,
            "vision_buffer" : self.vision_lightmap.get_lightmap_texture(),
            "height_buffer" : self.height_buffer,
            "reflect_buffer" : self.reflect_buffer,
            "reflect_map" : BGL.assets.get("CE-placeholder-art/texture/reflection_stars")
        })

    def render_static_lightbuffer_object(self,obj):
        """ Render an object in 'lightmap' space, with appropriate overrides for lights
        """
        obj.set_alt_camera(self.static_lightmap.camera)
        obj.render( True )
        obj.reset_alt_camera()

    def configure_dynamic_lightmapper(self):
        """ Configures the dynamic raytracer """
        self.dynamic_lights = []
        dynamic_lightmapper = Lightmapper( 
                lights = self.dynamic_lights, 
                geometry = self.get_occluders(), 
                camera = self.camera,
                width = self.dynamic_lightmap_width,
                height = self.dynamic_lightmap_height )

        return dynamic_lightmapper

    def configure_vision_lightmapper(self):
        """ Yada yada """
        self.player_lights = []
        vision_lightmapper = Lightmapper( 
                lights = self.player_lights,
                geometry = self.get_occluders(), 
                camera = self.camera,
                width = self.vision_lightmap_width,
                height = self.vision_lightmap_height )

        return vision_lightmapper

    def encode_light_objects(self, light_type):
        """ Converts Objects to Lightmapper compatible lights if appropriate
        """
        return list(map( lambda obj : { "position" : obj.p, "color" : obj.color, "radius" : obj.light_radius }, 
                         filter(lambda obj : obj.light_type == light_type, self.objects)))

    def encoder_player_lights( self ):
        return list(map(lambda player: { "position": player.p, "color" : [1.0,1.0,1.0,1.0], "radius" : player.sight_radius },[ self.player ]))

    def compute_dynamic_lightmap(self):
        """ Calculates the offscreen textures for the dynamic lightmap 
        """
        ### Raytrace lights
        self.dynamic_lights.clear()
        self.dynamic_lights.extend(self.encode_light_objects( Object.LightTypes.DYNAMIC_SHADOWCASTER ))
        self.dynamic_lightmap.compute()

        ### Sprite Lights
        with BGL.context.render_target(self.dynamic_lightmap.get_lightmap_framebuffer()):
            with BGL.blendmode.add:
                for obj in list(filter(lambda x: x.light_type == Object.LightTypes.DYNAMIC_TEXTURE_OVERLAY, self.objects)):
                    obj.render(True)

    def compute_vision_lightmap(self):
        """ Calculates the offscreen textures for the vision lightmap 
        """
        ### Raytrace lights
        self.player_lights.clear()
        self.player_lights.extend(self.encoder_player_lights() )
        self.vision_lightmap.compute()


    def compute_static_lightmap(self):
        """ Calculates the static (non dynamic lights) offscreen texture 
        """
        ### Raytrace lights
        static_shadowcaster_lights = self.encode_light_objects( Object.LightTypes.STATIC_SHADOWCASTER )
        lightmap = Lightmapper( lights = static_shadowcaster_lights, 
                             geometry = self.get_occluders(), 
                             camera = Camera( view = centered_view( self.width*2, self.height*2, Y_Axis = Y_Axis_Up ) ),
                             width=self.static_lightmap_width, 
                             height= self.static_lightmap_height )

        lightmap.compute()

        self.static_lightmap = lightmap
        ### Sprite Lights
        with BGL.context.render_target(lightmap.get_lightmap_framebuffer()):
            with BGL.blendmode.add:
                for obj in list(filter(lambda x: x.light_type == Object.LightTypes.STATIC_TEXTURE_OVERLAY, self.objects)):
                    self.render_static_lightbuffer_object( obj )

        return self.static_lightmap

    def render_tiles(self, channel = None):
        BGL.context.clear( 0.0,0.0,0.0,0.0 )
        self.tilemap.render(channel)

    def render_static_lightmap(self):
        """ Render the composited lightmap to the active target """
        self.primitive.render_shaded( self.shader, {
            "texBuffer"         : self.static_lightmap.get_lightmap_texture(),
            "translation_local" : [ 0, 0 ],
            "scale_local"       : [ self.width, self.height ],
            "view"              : self.camera.get_view(),
            "scale_world"       : self.camera.get_scale(),
            "translation_world" : self.camera.translate_position( [ 0.0,0.0] ),
            "rotation_local"    : 0.0,
            "filter_color"      : [ 1.0,1.0,1.0,1.0],
            "uv_translate"      : [ 0,0] 
        })
        

