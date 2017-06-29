from client.beagle.beagle_api import api as BGL
class ColorCycler(BGL.auto_configurable):
    def __init__(self, **kwargs):
        BGL.auto_configurable.__init__(self,
            {
                "channel_speeds" : [ 0.01,0.02, 0.03] 
            }, **kwargs)
    def tick(self):
        pass

    def drive(self,obj):
        for i in range(0,3):
            speed = self.channel_speeds[i]
            obj.color[i] = obj.color[i] + speed
            if(abs(obj.color[i]) > 1.0):
                self.channel_speeds[i] = self.channel_speeds[i] * -1
