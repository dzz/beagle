from client.math.curve import curve
from client.gfx.context import gfx_context
import client.system.log as log

class curve_sequencer:
    def __init__(self,config_template, scene_renderers = {} ):

        if( type(config_template) == type(self) ):
            config = config_template.config
        else:
            config = config_template

        self.config = config
        self.curves = {}
        self.slaves = []
        for sceneKey in self.config["curves"]:
            self.curves[sceneKey] = {}
            for curveKey in self.config["curves"][sceneKey]:
                self.curves[sceneKey][curveKey] = curve(self.config["curves"][sceneKey][curveKey])
        self.t                   = 0.0
        self.total_t             = self.config["start"]
        self.delta_t             = 1.0/60.0 * self.config["timescale"]
        self.current_scene_renderer = None
        self.current_scene_key      = None
        self.scene_renderers = scene_renderers
        self.runtime_vars   = { "scene.time": 0.0, "sequence.time" : 0.0 }
        self.started = False
        self.finished = False
        self.target_t = -1

    def seek_forward(self, t):
        self.target_t = t

    def register_slave(self,slave):
        self.slaves.append(slave)

    def register_slaves(self,slaves):
        for slave in slaves:
            self.register_slave(slave)

    def is_started(self):
        return self.started
    def is_finished(self):
        return self.finished

    def animated_value(self,key):
        if key in self.runtime_vars:
            return self.runtime_vars[key]

        if(self.current_scene_key):
            if key in self.curves[self.current_scene_key]:
                return self.curves[self.current_scene_key][key].value_at(self.t)
            else:
                log.write( log.WARNING, "animation curve '{0}' not found in current scene".format(key) )
                return 0.0
        return 0.0

    def get_scene_time(self):
        return self.t

    def get_aggregate_time(self):
        return self.total_t

    def get_delta_t(self):
        return self.delta_t

    def single_tick(self):
        for slave in self.slaves:
            slave.tick()

        self.started = True
        self.total_t += self.delta_t
        if(self.total_t > self.config["end"]):
            if "loop" in self.config and self.config["loop"]:
                self.total_t = 0.0
            else:
                self.finished = True
                next_scene_renderer = None
                return


        next_scene_renderer = None
        for sceneKey in self.config["scenes"]:
            sceneDef = self.config["scenes"][sceneKey]
            if self.total_t > sceneDef["start"] and self.total_t < sceneDef["end"]:
                self.t = self.total_t - sceneDef["start"]
                if sceneKey in self.scene_renderers:
                    next_scene_renderer = self.scene_renderers[sceneKey]
                self.current_scene_key = sceneKey
                break

        self.current_scene_renderer = next_scene_renderer

        self.runtime_vars["scene.time"] = self.t
        self.runtime_vars["sequence.time"] = self.total_t

    def tick(self):
        if(self.target_t == -1):
            self.single_tick()
            return
        while(self.total_t < self.target_t):
            self.single_tick()
        self.target_t = -1
        return self.total_t
    
    def is_finished(self):
        if( self.total_t > self.config["end"] ):
            return True

    def render(self):
        if(self.total_t > self.config["end"]):
            gfx_context.clear([0.0,0.0,0.0,1.0])
            return
        else:
            if(self.current_scene_renderer):
                self.current_scene_renderer()

