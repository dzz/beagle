from Beagle import API as BGL
from .LightMapper import LightMapper
from math import hypot, sin, cos
from random import uniform, shuffle
from ..GeometryUtils import ray_segment_intersection

class uniform_fade():
    primitive = BGL.primitive.unit_uv_square
    shader = BGL.assets.get("beagle-nl/shader/uniform_fade")

    def apply_fadeout( amount ):
        uniform_fade.primitive.render_shaded( uniform_fade.shader, { "amt" : amount } )

def ray_to_line( x,y,dx,dy, x1,y1,x2,y2):
    raylen = 1000.0
    return ray_segment_intersection([[ x-(dx*raylen), y-(dy*raylen)], [ x+(dx*raylen), y+(dy*raylen)]], [[ x1,y1], [x2,y1]] )

class PhotonMapper(BGL.auto_configurable):


    def __init__(self, **kwargs ):
        BGL.auto_configurable.__init__(self,{
            'geometry' : [ [[-1.0,-1.0],[1.0,1.0]], [[1.0,-1.0],[-1.0,1.0]] ],
            'emitters' : [ [ -4.0, -4.0, 1.0, 1.0 ] ],
            'camera' : None,
            'width' : 512,
            'height' : 512,
            'photon_radius' : 100.0,
            'photon_emitter_power' : 0.02,
            'photon_decay' : 0.98,
            'photon_decay_jitter' : 0.3,
            'photon_max_bounces' : 40,
            'num_photons' : 32,
            'photon_observe_chance' : 0.5,
            'stream' : False
        }, **kwargs );
        self.intersections = []

        self.trace_lightmapper = LightMapper( debug_texture_name = "photon-map", geometry = self.geometry, camera = self.camera, width = self.width, height = self.height )
        self.stage_photons()

        if self.stream is False:
            while len(self.staged_photons) > 0:
                self.compute_next()


    def get_texture(self):
        return self.trace_lightmapper.get_lightmap_texture()

    def trace_photon( self, photon ):
        generation = 1.0
        while generation < self.photon_max_bounces:
            c = 1.0 * self.photon_emitter_power;
            self.trace_lightmapper.set_lights([{
                "position" : [ photon[0], photon[1] ],
                "color" : [ c * photon[5][0],c*photon[5][1],c*photon[5][2],1.0 ],
                "radius" : self.photon_radius / generation
            }])

            if(uniform(0.0,1.0) < self.photon_observe_chance):
                self.trace_lightmapper.compute( False )
            photon[4] = photon[4] * self.photon_decay * uniform(1.0,1.0-self.photon_decay_jitter)
            r = uniform(-3.14,3.14)
            photon[2],photon[3] = cos(r) , sin(r)
            
            closest_intersection = None
            minimum_dist = None
            for line in self.transformed_geometry:
                try:
                    test_intersection = ray_to_line( photon[0], photon[1], photon[2], photon[3], line[0][0],line[0][1], line[1][0], line[1][1] )
                except ZeroDivisionError as e:
                    test_intersection = None
                    
                if(test_intersection):
                    dist = hypot( photon[0] - test_intersection[0], photon[1] - test_intersection[1] )
                    if not minimum_dist:
                        minimum_dist = dist
                        closest_intersection = test_intersection
                    else:
                        if(dist < minimum_dist):
                            minimum_dist = dist
                            closest_intersection = test_intersection 

            if closest_intersection is None:
                break
            else:
                generation = generation  + 1
                photon[0] = closest_intersection[0]
                photon[1] = closest_intersection[1]
                self.intersections.append([photon[0], photon[1]])


    def stage_photons(self):
        self.transformed_geometry = self.geometry
        self.trace_lightmapper.clear()
        self.staged_photons = []
        for emitter_def in self.emitters:
            for i in range(0,self.num_photons):
                    x , y = uniform( emitter_def[0], emitter_def[0] + emitter_def[2] ), uniform( emitter_def[1], emitter_def[1] + emitter_def[3]) 
                    power = 1.0
                    self.staged_photons.append([x,y,0.0,0.0,power, emitter_def[4]])
        shuffle(self.staged_photons)


    def compute_next(self):
        if len(self.staged_photons):
            photon = self.staged_photons.pop()
            if photon:
                self.trace_photon(photon)

 
