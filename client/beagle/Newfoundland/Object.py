from client.beagle.beagle_api import api as BGL
from .Drivers.StaticDriver import StaticDriver

class Object(BGL.basic_sprite_renderer, BGL.auto_configurable):
    class OccluderTypes:
        NONE = 0,
        LIGHT = 1,
        VISION = 2,
        ALL =3

    class TickTypes:
        STATIC = 0
        TICK_FOREVER = 1
        PURGING = 2

    class LightTypes:
        NONE = 0
        STATIC_SHADOWCASTER = 1
        DYNAMIC_SHADOWCASTER = 2
        STATIC_TEXTURE_OVERLAY = 3
        DYNAMIC_TEXTURE_OVERLAY = 4

    def __init__(self, **kwargs):
        BGL.auto_configurable.__init__( self,
            {
                'texture':BGL.assets.get("CE-placeholder-art/texture/default_object"),
                'occlusion_type' : Object.OccluderTypes.NONE,
                'occluder_geometry':[
                    #[ [0,0], [0,1] ],
                    #[ [1,1], [1,0] ]
                ],
                'p': [0,0],
                'size' : [1.0,1.0],
                'rad':0.0,
                'color' : [1.0,1.0,1.0,1.0],
                'light_type' : Object.LightTypes.NONE,
                'light_radius' : 5.0,
                'tick_type' : Object.TickTypes.PURGING,
                'visible' : True,
                'tick_owner' : None,
                'drivers' : [ StaticDriver() ],
                'alt_camera' : None
            }, **kwargs )

    def tick(self):
        for driver in self.drivers:
            driver.tick()
            driver.drive(self)
        return True

    def set_alt_camera(self, alt_camera):
        self.alt_camera = alt_camera

    def reset_alt_camera(self):
        self.alt_camera = None

    def get_camera(self):
        if self.alt_camera:
            return self.alt_camera
        return self.floor.camera

    def render(self, force_visible = False ):
        if force_visible or self.visible :
            BGL.basic_sprite_renderer.render(self)
            return

    def setFloor(self,floor):
        self.floor = floor


    def get_shader_params(self):
        return {
            "texBuffer"            : self.texture,
            "translation_local"    : [ 0, 0 ],
            "scale_local"          : self.size,
            "translation_world"    : self.get_camera().translate_position( self.p ),
            "scale_world"          : self.get_camera().get_scale(),
            "view"                 : self.get_camera().view,
            "rotation_local"       : self.rad,
            "filter_color"         : self.color,
            "uv_translate"         : [ 0,0 ] }
