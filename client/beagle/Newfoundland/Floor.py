from client.beagle.beagle_api import api as BGL
from .Object import Object
from .Renderers.FloorRenderer import FloorRenderer
from .FloorObjectTickManager import FloorObjectTickManager
from .Drivers.WobbleDriver import WobbleDriver
from .Drivers.ColorCycler import ColorCycler
from .Tilemap import Tilemap

class Floor(FloorRenderer, FloorObjectTickManager, BGL.auto_configurable):
    def __init__(self, **kwargs):
        BGL.auto_configurable.__init__(self,
            {
                'width':8.0,
                'height':8.0,
                'objects': self.get_default_objects(),
                'camera' : None,
                'tilemap' : Tilemap(),
                'player' : None
            }, **kwargs )

        FloorObjectTickManager.__init__(self)
        self.tilemap.linkFloor(self)
        self.player.floor = self
        for obj in self.objects:
            self.link_object(obj)
        FloorRenderer.__init__(self)

    def link_object(self, obj):
        """ Link an object to this floor
        """
        obj.setFloor(self)
        self.create_tickable(obj)

    def get_default_objects(self):
        """ Return a default 'scene' of objects
        """
        def testLightingRig():
            """ Make objects with lighting properties set
            """
            tex_radial = BGL.assets.get("NL-lights/texture/radial")
            tex_flare = BGL.assets.get("NL-lights/texture/flare")
            return [
                ###### ## static raytraced lights
                ##Object( visible = False, light_type = Object.LightTypes.STATIC_SHADOWCASTER,
                ##        p = [ -3, -3 ], light_radius=55.0, color = [ 0.6,0.1,0.0,1.0 ] ),

                ##Object( visible = False, light_type = Object.LightTypes.STATIC_SHADOWCASTER,
                ##        p = [ 3, -3 ], light_radius=15.0, color = [ 0.2,0.5,0.9,1.0 ] ),
                #### cheap (but artistic!) overlay sprite (notice size vs. radius, its just a sprite )

                ##Object( visible = False, light_type = Object.LightTypes.STATIC_TEXTURE_OVERLAY,
                ##         p = [ -9, 5 ], size = [12.0,12.0], color = [ 0.1,0.08,0.6,1.0 ], texture = tex_radial ),

                ##Object( visible = False, light_type = Object.LightTypes.STATIC_TEXTURE_OVERLAY,
                ##         p = [ 9, 5 ], size = [24.0,24.0], color = [ 0.1,0.08,0.6,1.0 ], texture = tex_flare ),

                ##Object( visible = False, light_type = Object.LightTypes.STATIC_TEXTURE_OVERLAY,
                ##         p = [ 9, 5 ], size = [24.0,24.0], color = [ 0.1,0.08,0.6,1.0 ], texture = tex_flare ),

                ##Object( visible = False, light_type = Object.LightTypes.STATIC_TEXTURE_OVERLAY,
                ##         p = [ 9, 5 ], size = [48.0,48.0], color = [ 0.3,0.8,0.6,1.0 ], texture = tex_flare ),

                #### ## expensive(fun) raytraced lights
###                Object( visible = False, light_type = Object.LightTypes.DYNAMIC_SHADOWCASTER,
###                        p = [ 0, 0 ], light_radius=34.0, color = [ 0.0,0.0,1.0,1.0 ], drivers = [ WobbleDriver() ] ),
###
###                Object( visible = False, light_type = Object.LightTypes.DYNAMIC_SHADOWCASTER,
###                        p = [ 0, 0 ], light_radius=17.0, color = [ 1.0,0.0,1.0,1.0 ], drivers = [ WobbleDriver( a=0.1,b=-0.2, radius=4.0) ] ),
###
                Object( visible = False, light_type = Object.LightTypes.DYNAMIC_SHADOWCASTER,
                        p = [ 0, 0 ], light_radius=20.0, color = [ 1.0,0.0,1.0,1.0 ], drivers = [ ColorCycler(), WobbleDriver( a=-0.3,b=0.4, radius=7.0) ] ),
                ##
                Object( visible = False, light_type = Object.LightTypes.DYNAMIC_TEXTURE_OVERLAY,
                        p = [ 0, 3 ], size=[14.5,14.5], color = [ 0.2,0.1,0.01,1.0 ], texture = tex_flare, drivers = [ WobbleDriver( a=-0.03,b=0.08, radius=4.0), ColorCycler() ] ),
            ]
        return testLightingRig()

    def tick(self):
        FloorObjectTickManager.tick(self)

    def get_photon_emitters(self):
        return self.tilemap.get_photon_emitters()

    def get_occluders(self):
        """ return (in floor-space) coordinates to occluder geometry
        """
        return self.tilemap.get_light_occluders()

