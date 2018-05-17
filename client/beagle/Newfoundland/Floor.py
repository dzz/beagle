import gc
from client.beagle.beagle_api import api as BGL
from .Object import Object
from .Renderers.FloorRenderer import FloorRenderer
from .FloorObjectTickManager import FloorObjectTickManager
from .Drivers.WobbleDriver import WobbleDriver
from .Drivers.ColorCycler import ColorCycler
from .Tilemap import Tilemap
from .FloorPhysics import FloorPhysics

def createFloorClass( Renderer ):
    class Floor(Renderer, FloorObjectTickManager, BGL.auto_configurable, FloorPhysics):

        def __del__(self):
            self.objects = []
            if(self.physics_space):
                self.physics_space.bodies = []
                self.physics_space.shapes = []
                gc.collect()
            
        def __init__(self, **kwargs):
            BGL.auto_configurable.__init__(self,
                {
                    'width':8.0,
                    'height':8.0,
                    'objects': self.get_default_objects(),
                    'camera' : None,
                    'tilemap' : Tilemap(),
                    'player' : None,
                    'renderer_config' : {},
                    'physics' : {
                        'wall_friction' : 0.05,
                        'timestep_divisions' : 100.0
                    }
                }, **kwargs )
    

            FloorObjectTickManager.__init__(self)
            self.tilemap.linkFloor(self)
            for obj in self.objects:
                self.link_object(obj)
            self.link_players()
            self._tick = 0.0
            Renderer.__init__(self, **self.renderer_config)
            if(self.physics):
                FloorPhysics.__init__(self)
    
        def link_players(self):
            self.player.floor = self
            self.objects.append( self.player )
            self.player.link_floor()
    
        def create_object(self,obj):
            self.objects.append(obj)
            self.link_object(obj)
            if(obj.physics is not None):
                self.register_new_physics_object(obj)
            obj.link_floor()

        def remove_object(self,obj):
            if obj in self.objects:
                self.objects.remove(obj)
                if self.physics_space:
                    if obj.body:
                        print("removing ",obj.body)
                        self.physics_space.bodies.remove(obj.body)
                        obj.body.destroy()
                #if obj in self.simple_tick_manager.tickables:
                #    self.simple_tick_manager.tickables.remove(obj)
                #if obj in self.purging_tick_manager.tickables:
                #    self.purging_tick_manager.tickables.remove(obj)
            else:
                print("WARNING, DUPLICATED ATTEMPTED REMOVAL OF {0}".format(obj))
 
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
    
            # return testLightingRig()
            return []
    
        def tick(self):
            if( self.physics):
                FloorPhysics.tick(self)
            FloorObjectTickManager.tick(self)
            self._tick = self._tick+1.0
    
        def get_photon_emitters(self):
            return self.tilemap.get_photon_emitters()
    
        def get_physics_occluders(self):
            return self.get_occluders()

        def get_occluders(self):
            """ return (in floor-space) coordinates to occluder geometry
            """
            return self.get_light_occluders()

        def get_light_occluders(self):
            return self.tilemap.get_light_occluders()

        
    return Floor

Floor = createFloorClass( FloorRenderer ) 
