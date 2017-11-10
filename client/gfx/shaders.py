import client.beagle.beagle_environment  as beagle_environment
import client.system.log as log
import hwgfx
from client.gfx.texture import *
from client.gfx.framebuffer import framebuffer
import re
import os

#_shaders = {}
#_reloadables = []

def read_shader_file(path):

    f = open(path)
    data = f.read()
    f.close()

    regex = r"\/\/\s*@load\s*\"(.*)\""
    matches = re.finditer(regex, data)

    replaced = data
    for matchNum, match in enumerate(matches):
        filename = match.groups()[0]
        search = match.group(0)
        fullpath = os.path.dirname(path) + "//" + filename
        ifile = open(fullpath)
        contents = ifile.read()
        ifile.close()
        replaced = replaced.replace(search,contents)

    return replaced

def build_params(dict):
    params = []
    for key in dict:
            params.append( (key, dict[key]) )

def get_unique(vert,frag, path=None):
    return shader(vert,frag,path)


def reload():
    return
    #global _reloadables
    #for shader in _reloadables:
    #    shader.reload()

def get(vert,frag, path = None ):
    #global _shaders
    #if (vert,frag,path) in _shaders.keys():
    #    return _shaders[ (vert, frag, path) ]
    #else:
    loaded                  = shader(vert,frag,path) 
    return loaded

def get_client_program( vert, frag, root_dir = None, unique = False ):
    if root_dir is None:
        root_dir = beagle_environment.get("app_dir") + "shaders/"

    if unique:
        return get_unique(vert, frag, root_dir )
    else:
        return get(vert,frag, root_dir )

def get_unique_client_program( vert, frag, root_dir = None ):
    return get_client_program(vert,frag, root_dir)


class shader(object):
    def __init__(self,vert,frag, path = None, compile = False ):
        #global _reloadables

        self.last_bound = {}
        if not compile:
            #use shader_load to pull from filesystem
            #### if path is not None:
            ####     log.write(log.DEBUG,"Compiling USER shader: {0}{1},{2}".format(path,vert,frag))
            #### else:
            ####     log.write(log.DEBUG,"Compiling ENGINE shader: {0},{1}".format(vert,frag))
            if( path is None):
                vpath =  "shaders/" + vert + ".vert.glsl"
                fpath =  "shaders/" + frag + ".frag.glsl"
            else:
                vpath =  path + vert + ".glsl"
                fpath =  path + frag + ".glsl"


            vsrc = read_shader_file(vpath)
            fsrc = read_shader_file(fpath)
 
            self._shader = hwgfx.shader_compile( vsrc, fsrc )
            self.vpath = vpath
            self.fpath = fpath
            #_reloadables.append( self )
        else:
            #shader compile the strings
            self._shader = hwgfx.shader_compile(vert,frag)

        log.write(log.DEBUG,"Linked program:{3}".format(path,vert,frag,self._shader))

    def reload(self):
        hwgfx.shader_drop(self._shader)
        self._shader = hwgfx.shader_load(self.vpath,self.fpath)

    def transform_uniforms(u):
        uniforms = []
        for key in u:
            uniforms.append( (key, u[key]) )
        return uniforms

    def bind(self,uniforms = {}, bind_textures = True, reserved_units = 0 ):
        hwgfx.shader_bind(self._shader);

        tex_unit = reserved_units

        for key in uniforms:
            name    = key
            vector  = uniforms[key]


            vlen = 0
            if type(vector) is list:
                ## this doesn't work...not sure why....

                ##if name in self.last_bound:
                ##    if self.last_bound[name] == vector:
                ##        continue
                ##self.last_bound[name] = vector
                vlen    = len(vector)

            if vlen != 0:
                if name in self.last_bound:
                    if self.last_bound[name] == vector:
                        continue

            if vlen == 1:
                if type(vector[0]) == int:
                    hwgfx.shader_bind_int( self._shader, name, vector[0] )
                elif type(vector[0]) == list:
                    hwgfx.shader_bind_floats( self._shader, name, vector[0] )
                else:
                    hwgfx.shader_bind_float (self._shader, name, 
                            vector[0])
            elif vlen ==2:
                hwgfx.shader_bind_vec2  (self._shader, name, 
                        vector[0],
                        vector[1])
            elif vlen ==3:
                hwgfx.shader_bind_vec3  (self._shader, name, 
                        vector[0],
                        vector[1],
                        vector[2])
            elif vlen ==4:
                hwgfx.shader_bind_vec4  (self._shader, name, 
                        vector[0],
                        vector[1],
                        vector[2],
                        vector[3])
            elif type(vector) is texture:
                if bind_textures:
                    vector.bind(tex_unit)
                    tex_unit += 1
                if name in self.last_bound:
                    if self.last_bound[name] == vector:
                        continue
                hwgfx.shader_bind_texture( self._shader, name, vector._tex )
            elif type(vector) is framebuffer:
                if bind_textures:
                    vector.get_texture().bind(tex_unit)
                    tex_unit += 1
                if name in self.last_bound:
                    if self.last_bound[name] == vector:
                        continue
                hwgfx.shader_bind_texture( self._shader, name, vector.get_texture()._tex )
            elif type(vector) is float:
                if name in self.last_bound:
                    if self.last_bound[name] == vector:
                        continue
                hwgfx.shader_bind_float (self._shader, name, 
                        vector)

        self.last_bound = uniforms

    def __del__(self):
        #log.write(log.DEBUG, "Deleting shader program {0}".format(self._shader))
        hwgfx.shader_drop(self._shader)

    @classmethod
    def from_core_library( cls, vert, frag):
        return cls( vert, frag , "shaders/" )

    @classmethod
    def from_application_library( cls, vert, frag):
        return cls( vert, frag, host_config.get("app_dir") + "shaders/" )


