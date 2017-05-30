from client.beagle.beagle_api import api as BGL 

class Camera(BGL.auto_configurable):
    def __init__(self, **kwargs):
        BGL.auto_configurable.__init__(self, 
            {
                "p" : [ 0.0, 0.0 ],
                "zoom" : 1.0,
                "view" : BGL.assets.get("CE-camera/coordsys/default_view" )
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


