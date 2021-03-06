from Beagle import API as BGL
from client.gfx.coordinates import centered_view,Y_Axis_Up

from ..Camera import Camera
from ..Object import Object
from .LightMapper import LightMapper
from .PhotonMapper import PhotonMapper

class FloorRenderer(BGL.auto_configurable):
    """ Renderer class for composing with Floor 

        Attributes:
            pixel_copy_shader: static instance to the shader used for copying the compositing stages
    """
    pixel_copy_shader = BGL.assets.get("beagle-2d/shader/passthru")
    compositor_shader = BGL.assets.get("beagle-nl/shader/floor-compositor")

    def __init__(self, **kwargs ):
        BGL.auto_configurable.__init__(self, {
            "static_lightmap_width" : 512,
            "static_lightmap_height" : 512,
            "dynamic_lightmap_width" : 256,
            "dynamic_lightmap_height" :256,
            "vision_lightmap_width" : 512,
            "vision_lightmap_height" : 512,
            "photon_map_width" : 512,
            "photon_map_height" : 512,
            "compositor_shader" : None,
            "photon_mapper_config" : {} 
        }, **kwargs);


        ## used to place composited elements
        self.primitive = BGL.primitive.unit_uv_square
        self.shader = BGL.assets.get("beagle-2d/shader/beagle-2d")

        self.configure_lightmaps()
        self.create_compositing_buffers()
        ##self.photon_buffer = BGL.framebuffer.from_screen()
        ##self.floor_buffer = BGL.framebuffer.from_screen()
        ##self.light_buffer = BGL.framebuffer.from_dims(256,256)
        ##self.object_buffer = BGL.framebuffer.from_screen()
        ##self.height_buffer = BGL.framebuffer.from_screen()
        ##self.reflect_buffer = BGL.framebuffer.from_dims(256,256)
        ##self.vision_buffer = BGL.framebuffer.from_dims(512,512)


    def configure_lightmaps(self):
        self.photon_map = self.compute_photon_map()
        self.static_lightmap = self.compute_static_lightmap()
        self.static_lightmap.get_lightmap_texture().debugger_attach("static-lightmap")
        self.dynamic_lightmap = self.configure_dynamic_lightmapper()
        self.dynamic_lightmap.get_lightmap_texture().debugger_attach("dynamic-lightmap")
        self.vision_lightmap = self.configure_vision_lightmapper()
        self.vision_lightmap.get_lightmap_texture().debugger_attach("vision-lightmap")

    def create_compositing_buffers(self):
        self.photon_buffer = BGL.framebuffer.from_screen()
        self.floor_buffer = BGL.framebuffer.from_screen()
        self.light_buffer = BGL.framebuffer.from_dims(256,256)
        self.object_buffer = BGL.framebuffer.from_screen()
        self.height_buffer = BGL.framebuffer.from_screen()
        self.reflect_buffer = BGL.framebuffer.from_dims(256,256)
        #self.vision_buffer = BGL.framebuffer.from_dims(512,512)

    def precompute_frame(self):
        """ Pre-render compositing """

        if self.photon_map:
            self.photon_map.compute_next()
        self.compute_vision_lightmap()
        self.compute_dynamic_lightmap()

        with BGL.context.render_target( self.floor_buffer ):
            with BGL.blendmode.alpha_over:
                self.render_tiles()
        with BGL.context.render_target( self.height_buffer ):
            self.render_tiles("height")
        with BGL.context.render_target( self.reflect_buffer ):
            self.render_tiles("reflection")

        with BGL.context.render_target( self.photon_buffer ):
            BGL.context.clear(0.0,0.0,0.0,1.0)
            self.render_photon_map()

        with BGL.context.render_target( self.light_buffer ):
            BGL.context.clear(0.0,0.0,0.0,1.0)
            with BGL.blendmode.add:
                self.render_static_lightmap()
                self.dynamic_lightmap.get_lightmap_framebuffer().render_processed( FloorRenderer.pixel_copy_shader )

        with BGL.context.render_target( self.object_buffer ):
            BGL.context.clear(0.0,0.0,0.0,0.0)
            with BGL.blendmode.alpha_over:
                self.render_objects()
                #for intersection in PhotonMapper.intersections:
                #    obj = Object( p = intersection  )
                #    obj.setFloor(self)
                #    obj.render()
                    
            #self.player.render() 

    def get_player_objects(self):
        return [ self.player ]

    def render_objects(self):
        """ Render floor objects """
        objects = []
        objects.extend( self.objects )
        objects.extend( self.get_player_objects() )

        renderable_objects = list(filter(lambda x: x.visible, objects))
        renderable_objects.sort( key = lambda x: x.p[1] )
        renderable_objects.sort( key = lambda x: x.z_index )
        for obj in renderable_objects:
            obj.render()

    def render_composite(self):
        if self.compositor_shader:
            shader  = self.compositor_shader
        else:
            shader = FloorRenderer.compositor_shader

        BGL.compositor.render_composite( shader, {
            "tick"              : self._tick,
            "camera_position" : self.camera.p,
            "floor_buffer" : self.floor_buffer,
            "light_buffer" : self.light_buffer,
            "object_buffer" : self.object_buffer,
            "vision_buffer" : self.vision_lightmap.get_lightmap_texture(),
            "photon_buffer" : self.photon_buffer,
            "height_buffer" : self.height_buffer,
            "reflect_buffer" : self.reflect_buffer,
            "reflect_map" : self.photon_map.get_texture(),
        })

    def render(self):
        """ Perform final composite to active target """
        self.precompute_frame()
        self.render_composite()


    def render_static_lightbuffer_object(self,obj):
        """ Render an object in 'lightmap' space, with appropriate overrides for lights
        """
        obj.set_alt_camera(self.static_lightmap.camera)
        obj.render( True )
        obj.reset_alt_camera()

    def configure_dynamic_lightmapper(self):
        """ Configures the dynamic raytracer """
        self.dynamic_lights = []
        dynamic_lightmapper = LightMapper( 
                lights = self.dynamic_lights, 
                geometry = self.get_occluders(), 
                camera = self.camera,
                width = self.dynamic_lightmap_width,
                height = self.dynamic_lightmap_height )

        return dynamic_lightmapper

    def configure_vision_lightmapper(self):
        """ Yada yada """
        self.player_lights = []
        vision_lightmapper = LightMapper( 
                lights = self.player_lights,
                geometry = self.get_occluders(), 
                camera = self.camera,
                width = self.vision_lightmap_width,
                height = self.vision_lightmap_height )

        return vision_lightmapper

    def encode_light_objects(self, light_type):
        """ Converts Objects to LightMapper compatible lights if appropriate
        """
        return list(map( lambda obj : { "position" : obj.p, "color" : obj.light_color, "radius" : obj.light_radius }, 
                         filter(lambda obj : obj.light_type == light_type, self.objects)))

    def encode_player_lights( self ):
        return list(map(lambda player: { "position": player.p, "color" : [1.0,1.0,1.0,1.0], "radius" : player.sight_radius },self.get_player_objects()))

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
                    tmp = None
                    if(obj.light_texture):
                        tmp = obj.texture
                        obj.texture = obj.light_texture
                    obj.render(True)
                    if tmp:
                        obj.texture = tmp

    def compute_vision_lightmap(self):
        """ Calculates the offscreen textures for the vision lightmap 
        """
        ### Raytrace lights
        self.player_lights.clear()
        self.player_lights.extend(self.encode_player_lights() )
        self.vision_lightmap.compute()


    def compute_photon_map(self):
        defaults = {
                                    "emitters" : self.get_photon_emitters(),
                                    "geometry" : self.get_light_occluders(),
                                    "camera": Camera( view = centered_view( self.width*2, self.height*2, Y_Axis = Y_Axis_Up ) ),
                                    "width": self.photon_map_width,
                                    "height": self.photon_map_height 
        }
        defaults.update( self.photon_mapper_config )
        photon_map = PhotonMapper(**defaults)
        photon_map.compute_next()
        return photon_map

    def compute_static_lightmap(self):
        """ Calculates the static (non dynamic lights) offscreen texture 
        """
        ### Raytrace lights
        static_shadowcaster_lights = self.encode_light_objects( Object.LightTypes.STATIC_SHADOWCASTER )
        lightmap = LightMapper( lights = static_shadowcaster_lights, 
                             geometry = self.get_light_occluders(), 
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
        #BGL.context.clear( 0.0,0.0,0.0,0.0 )
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

    def render_photon_map(self):
        self.primitive.render_shaded( self.shader, {
            "texBuffer"         : self.photon_map.get_texture(),
            "translation_local" : [ 0, 0 ],
            "scale_local"       : [ self.width, self.height ],
            "view"              : self.camera.get_view(),
            "scale_world"       : self.camera.get_scale(),
            "translation_world" : self.camera.translate_position( [ 0.0,0.0] ),
            "rotation_local"    : 0.0,
            "filter_color"      : [ 1.0,1.0,1.0,1.0],
            "uv_translate"      : [ 0,0] 
        })
        

