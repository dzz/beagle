from client.beagle.beagle_api import api as BGL

from math import sin, cos

class WobbleDriver(BGL.auto_configurable):
    """ Wobbles your Object
    """
    def __init__(self, **kwargs):
        BGL.auto_configurable.__init__(self,
            {
                "t" : 0.0,
                "a" : 1.8,
                "b" : 0.8,
                "radius" : 17.0 }, **kwargs)

    def tick(self):
        self.t += 1.0 / 60.0 

    def drive( self, obj ):
        obj.p[0] = sin( self.t * self.a ) * self.radius
        obj.p[1] = cos( self.t * self.b ) * self.radius
         
