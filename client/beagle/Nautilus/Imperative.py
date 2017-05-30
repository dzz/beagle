from client.beagle.beagle_api import api as BGL
class Imperative(BGL.auto_configurable):
    """ An overrideable goal for ai tracking
    """
    def __init__(self,**kwargs):
        BGL.auto_configurable.__init__(self,{
            'goal' : [0.0, 0.0],
            'duration' : 1
        },**kwargs)

    def get_goal(self):
        """ static goal destination
        """
        return self.goal

    def finished(self):
        """ Timeout based completion condition
        """
        self.duration -= 1
        return self.duration <= 0
