from Beagle import API as BGL
from .LightMapper import LightMapper
from math import hypot, sin, cos
from random import uniform

class uniform_fade():
    primitive = BGL.primitive.unit_uv_square
    shader = BGL.assets.get("beagle-nl/shader/uniform_fade")

    def apply_fadeout( amount ):
        uniform_fade.primitive.render_shaded( uniform_fade.shader, { "amt" : amount } )

def line_intersection(line1, line2):
    xdiff = (line1[0][0] - line1[1][0], line2[0][0] - line2[1][0])
    ydiff = (line1[0][1] - line1[1][1], line2[0][1] - line2[1][1]) #Typo was here

    def det(a, b):
        return a[0] * b[1] - a[1] * b[0]

    div = det(xdiff, ydiff)
    if div == 0:
       return None

    d = (det(*line1), det(*line2))
    x = det(d, xdiff) / div
    y = det(d, ydiff) / div
    return [ x, y ]


def ray_to_line( x,y,dx,dy, x1,y1,x2,y2):

    raylen = 1000.0
    
    return line_intersection([[ x-(dx*raylen), y-(dy*raylen)], [ x+(dx*raylen), y+(dy*raylen)]], [[ x1,y1], [x2,y1]] )

    #r = 0.0
    #s = 0.0
    #d = 0.0
    #if (dy / dx != (y2 - y1) / (x2 - x1)):
    #    d = ((dx * (y2 - y1)) - dy * (x2 - x1))
    #if (d != 0):
    #    r = (((y - y1) * (x2 - x1)) - (x - x1) * (y2 - y1)) / d
    #    s = (((y - y1) * dx) - (x - x1) * dy) / d
    #    if (r >= 0 and s >= 0 and s <= 1):
    #        return [ ((x2-x1)*s)+x1, ((y2-y1)*s)+y1 ]
    #
class PhotonMapper(BGL.auto_configurable):
    photon_radius = 60.0
    photon_emitter_power = 0.01
    photon_decay = 0.8
    photon_decay_jitter = 0.3
    photon_max_bounces = 15
    num_photons = 60
    intersections = []

    def __init__(self, **kwargs ):
        BGL.auto_configurable.__init__(self,{
            'geometry' : [ [[-1.0,-1.0],[1.0,1.0]], [[1.0,-1.0],[-1.0,1.0]] ],
            'emitters' : [ [ -4.0, -4.0, 1.0, 1.0 ] ],
            'camera' : None,
            'width' : 512,
            'height' : 512
        }, **kwargs );

        self.trace_lightmapper = LightMapper( debug_texture_name = "photon-map", geometry = self.geometry, camera = self.camera, width = self.width, height = self.height )
        self.compute()


    def get_texture(self):
        return self.trace_lightmapper.get_lightmap_texture()

    def trace_photon( self, photon ):
        generation = 1.0
        while generation < PhotonMapper.photon_max_bounces:
            c = 1.0 * PhotonMapper.photon_emitter_power;
            self.trace_lightmapper.set_lights([{
                "position" : [ photon[0], photon[1] ],
                "color" : [ c,c,c,1.0 ],
                "radius" : PhotonMapper.photon_radius / generation
            }])
            self.trace_lightmapper.compute( False )
            photon[4] = photon[4] * PhotonMapper.photon_decay * uniform(1.0,1.0-PhotonMapper.photon_decay_jitter)
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
                PhotonMapper.intersections.append([photon[0], photon[1]])
                print(photon) 


    def compute(self):
        self.transformed_geometry = self.geometry
        self.trace_lightmapper.clear()
        for emitter_area in self.emitters:

            print("DOING : {0}".format(emitter_area))

            for i in range(0,PhotonMapper.num_photons):
                    print("Tracing Photon {0}".format(i))
                    x , y = uniform( emitter_area[0], emitter_area[0] + emitter_area[2] ), uniform( emitter_area[1], emitter_area[1] + emitter_area[3]) 
                    power = 1.0
                    self.trace_photon( [x,y,0.0,0.0,power] )
            print("Photon Traced {0} intersections" . format (len(PhotonMapper.intersections)) )

 
