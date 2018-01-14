from client.beagle.beagle_api import api as BGL 

class Camera(BGL.auto_configurable):
    def __init__(self, **kwargs):
        BGL.auto_configurable.__init__(self, 
            {
                "p" : [ 0.0, 0.0 ],
                "zoom" : 1.0,
                "view" : BGL.assets.get("NL-camera/coordsys/default_view" ),
                "near_light_threshold" : 200,
                "far_light_threshold" : 500,
            }, **kwargs );

    def translate_position(self, p):
        return [ p[0] - self.p[0], p[1] - self.p[1] ]

    def get_view(self):
        return self.view

    def get_zoom(self):
        return self.zoom

    def get_scale(self):
        w = self.zoom
        return [ w, w ]

    def get_uniform_scale(self):
        return self.get_scale()[0]

    def tick(self):
        pass

    def visible_occluders(self,native_lines):
        pass

    def visible_lights(self, lights):

        nlights = []
        for light in lights:
            dx = self.p[0] - light["position"][0]
            dy = self.p[1] - light["position"][1]

            sd = (dx*dx)+(dy*dy)

            if(sd < self.near_light_threshold):
                nlights.append(light)
            elif(sd < self.far_light_threshold):
                rad = ((self.far_light_threshold-sd)/(self.far_light_threshold-self.near_light_threshold))
                nlight = {}
                nlight['position'] = list(light['position'])
                nlight['color'] = list(light['color'])
                nlight['radius'] = light['radius']*rad
                nlight['color'][0]*=rad
                nlight['color'][1]*=rad
                nlight['color'][2]*=rad
                nlights.append(nlight)

        return nlights


