from client.beagle.beagle_api import api as BGL
from .Object import Object
from .Renderers.FloorRenderer import FloorRenderer
from .FloorObjectTickManager import FloorObjectTickManager
from .Drivers.WobbleDriver import WobbleDriver
from .Drivers.ColorCycler import ColorCycler
from .Tilemap import Tilemap
from .Dummy import Dummy
from .Door import Door
from .Drivers.TrackerDriver import TrackerDriver
from .CollisionPebble import CollisionPebble

class Floor(FloorRenderer, FloorObjectTickManager, BGL.auto_configurable):
    """ Represents a floor in a building
    """
    def __init__(self, **kwargs):
        BGL.auto_configurable.__init__(self,
            {
                'width':75.0,
                'height':75.0,
                'objects': self.get_default_objects(),
                'camera' : None,
                'building' : None,
                'tilemap' : Tilemap()
            }, **kwargs )
        FloorObjectTickManager.__init__(self)
        self.pebble = CollisionPebble()

        self.tilemap.linkFloor(self)
        #gotta love some O(n^2) linking
        for obj in self.objects:
            self.link_object(obj)

    def post_check(self):
        self.pebble.run_checks()

    def link_object(self, obj):
        """ Link an object to this floor
        """
        obj.setFloor(self)
        self.create_tickable(obj)

    def link_building(self, building):
        """ Link this floor to a parent building
        """
        self.building = building
        for p in self.building.players:
            p.setFloor(self)
        FloorRenderer.__init__(self)

    def get_default_objects(self):
        """ Return a default 'scene' of objects
        """
        def testLightingRig():
            """ Make objects with lighting properties set
            """
            tex_radial = BGL.assets.get("CE-lights/texture/radial")
            tex_flare = BGL.assets.get("CE-lights/texture/flare")
            return [
                #### Object( size = [1.0,1.0], p = [ -5.0, 5.0 ] ),
                #### Object( size = [1.0,1.0], p = [ 0.0, 0.0 ] ),
                #### Object( size = [1.0,1.0], p = [ 5.0, -5.0 ] ),
                #### ## static raytraced lights
                Object( visible = False, light_type = Object.LightTypes.STATIC_SHADOWCASTER,
                        p = [ -3, -3 ], light_radius=55.0, color = [ 0.6,0.1,0.0,1.0 ] ),

                Object( visible = False, light_type = Object.LightTypes.STATIC_SHADOWCASTER,
                        p = [ 3, -3 ], light_radius=15.0, color = [ 0.2,0.5,0.9,1.0 ] ),
                ## cheap (but artistic!) overlay sprite (notice size vs. radius, its just a sprite )

                Object( visible = False, light_type = Object.LightTypes.STATIC_TEXTURE_OVERLAY,
                         p = [ -9, 5 ], size = [12.0,12.0], color = [ 0.1,0.08,0.6,1.0 ], texture = tex_radial ),

                Object( visible = False, light_type = Object.LightTypes.STATIC_TEXTURE_OVERLAY,
                         p = [ 9, 5 ], size = [24.0,24.0], color = [ 0.1,0.08,0.6,1.0 ], texture = tex_flare ),

                Object( visible = False, light_type = Object.LightTypes.STATIC_TEXTURE_OVERLAY,
                         p = [ 9, 5 ], size = [24.0,24.0], color = [ 0.1,0.08,0.6,1.0 ], texture = tex_flare ),

                Object( visible = False, light_type = Object.LightTypes.STATIC_TEXTURE_OVERLAY,
                         p = [ 9, 5 ], size = [48.0,48.0], color = [ 0.3,0.8,0.6,1.0 ], texture = tex_flare ),

                #### ## expensive(fun) raytraced lights
                Object( visible = False, light_type = Object.LightTypes.DYNAMIC_SHADOWCASTER,
                        p = [ 0, 0 ], light_radius=34.0, color = [ 0.0,0.0,1.0,1.0 ], drivers = [ WobbleDriver() ] ),

                Object( visible = False, light_type = Object.LightTypes.DYNAMIC_SHADOWCASTER,
                        p = [ 0, 0 ], light_radius=17.0, color = [ 1.0,0.0,1.0,1.0 ], drivers = [ WobbleDriver( a=0.1,b=-0.2, radius=4.0) ] ),

                Object( visible = False, light_type = Object.LightTypes.DYNAMIC_SHADOWCASTER,
                        p = [ 0, 0 ], light_radius=20.0, color = [ 1.0,0.0,1.0,1.0 ], drivers = [ ColorCycler(), WobbleDriver( a=-0.3,b=0.4, radius=7.0) ] ),
                ##
                Object( visible = False, light_type = Object.LightTypes.DYNAMIC_TEXTURE_OVERLAY,
                        p = [ 0, 3 ], size=[14.5,14.5], color = [ 0.2,0.1,0.01,1.0 ], texture = tex_flare, drivers = [ WobbleDriver( a=-0.03,b=0.08, radius=4.0), ColorCycler() ] ),
                ## enemies
                Dummy(p = [-15.0, -15.0]),
                Dummy(p = [-10.0, -8.0],),
                Door(p = [-20.0, -10.0], open_coord = [-15.0, -10.0])
            ]
        return testLightingRig()

    def tick(self):
        #FloorRenderer.precompute_frame(self)
        FloorObjectTickManager.tick(self)

    def get_occluders(self):
        """ return (in floor-space) coordinates to occluder geometry
        """
        print(self.tilemap.get_light_occluders())

        return self.tilemap.get_light_occluders()

    def is_visible(self):
        """ if this floor is visible/active
        """
        return True
