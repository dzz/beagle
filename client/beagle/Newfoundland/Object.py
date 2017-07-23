from client.beagle.beagle_api import api as BGL
from .Drivers.StaticDriver import StaticDriver
import copy
from itertools import groupby

class GuppyRenderer():
    def __init__(self):
        pass

    def renderObjects(self,objects):

        #print("STARTING PASSES")
        passcount = 0
        objects.sort( key = lambda x: x.p[1] )
        objects.sort( key = lambda x: x.z_index )
        for zindex, layer in groupby( objects, lambda x: x.z_index ):
            for texture, renderpass in groupby( layer, lambda x: x.texture._tex ):
                passcount = passcount+1
                #print("PASS:",passcount,texture,zindex)
                for obj in list(renderpass):
                    #print(obj.__class__.__name__)
                    obj.render()
        #print("DONE PASS")
        

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
                'texture':BGL.assets.get("NL-placeholder/texture/arena"),
                'occlusion_type' : Object.OccluderTypes.NONE,
                'occluder_geometry':[
                    #[ [0,0], [0,1] ],
                    #[ [1,1], [1,0] ]
                ],
                'p': [0,0],
                'v': [0,0],
                'size' : [1.0,1.0],
                'rad':0.0,
                'color' : [1.0,1.0,1.0,1.0],
                'light_type' : Object.LightTypes.NONE,
                'light_radius' : 5.0,
                'tick_ratio' : 1.0,
                'tick_type' : Object.TickTypes.PURGING,
                'visible' : True,
                'light_color' : None,
                'tick_owner' : None,
                'drivers' : [ StaticDriver() ],
                'alt_camera' : None,
                'z_index' : 0,
                'snapshot_fields' : [ 'p' ],
                'collides_with_walls' : False,
                'record_snapshots' : False,
                'physics' : None
            }, **kwargs )

        if not self.light_color:
            self.light_color = self.color

        self._ticks = 0.0
        self._render_p = self.p
        self.customize()
        self.take_snapshot()

    def customize(self):
        pass

    def link_floor(self):
        pass

    def take_snapshot(self):
        snapshot = {}
        for field in self.snapshot_fields:
            snapshot[field] = copy.copy( self.__dict__[field] )

        self.snapshot = snapshot

    def tick(self):
        self._ticks = self._ticks + self.tick_ratio

        self.update_lerped()
        if(self._ticks >= 1.0):
            self._ticks = self._ticks - 1.0
            for driver in self.drivers:
                driver.tick()
                driver.drive(self)
            if self.record_snapshots:
                self.take_snapshot()
            return True

    def set_alt_camera(self, alt_camera):
        self.alt_camera = alt_camera

    def reset_alt_camera(self):
        self.alt_camera = None

    def get_camera(self):
        if self.alt_camera:
            return self.alt_camera
        return self.floor.camera

    def should_draw(self):
        return True

    def render(self, force_visible = False ):
        if force_visible or (self.visible and self.should_draw()):
            BGL.basic_sprite_renderer.render(self)
            return

    def setFloor(self,floor):
        self.floor = floor


    def get_p(self):
        if self.record_snapshots:
            return self.snapshot['p']
        return self.p

    def update_lerped(self):
        lerped = [
            ((1.0 - self._ticks) * self.get_p()[0]) + (self._ticks * self.p[0]),
            ((1.0 - self._ticks) * self.get_p()[1]) + (self._ticks * self.p[1]),
        ]

        self._render_p[0] = self._render_p[0] * 0.8 + lerped[0]*0.2
        self._render_p[1] = self._render_p[1] * 0.8 + lerped[1]*0.2

    def get_render_p(self):
        return self._render_p

    def get_shader_params(self):
        return {
            "texBuffer"            : self.texture,
            "translation_local"    : [ 0, 0 ],
            "scale_local"          : self.size,
            "translation_world"    : self.get_camera().translate_position( self.get_render_p() ),
            "scale_world"          : self.get_camera().get_scale(),
            "view"                 : self.get_camera().view,
            "rotation_local"       : self.rad,
            "filter_color"         : self.color,
            "uv_translate"         : [ 0,0 ] }

