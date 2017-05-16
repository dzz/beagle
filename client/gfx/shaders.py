import client.beagle.beagle_environment  as beagle_environment
import client.system.log as log
import hwgfx
from client.gfx.texture import *
from client.gfx.framebuffer import framebuffer

_shaders = {}
_reloadables = []

def build_params(dict):
    params = []
    for key in dict:
            params.append( (key, dict[key]) )

def get_unique(vert,frag, path=None):
    return shader(vert,frag,path)


def reload():
    global _reloadables
    for shader in _reloadables:
        shader.reload()

def get(vert,frag, path = None ):
    global _shaders
    if (vert,frag,path) in _shaders.keys():
        return _shaders[ (vert, frag, path) ]
    else:
        loaded                  = shader(vert,frag,path) 
        _shaders[( vert, frag, path )] = loaded
        return loaded

def get_client_program( vert, frag ):
    return get(vert,frag, beagle_environment.get("app_dir") + "shaders/")

def get_unique_client_program( vert, frag ):
    return get_unique(vert,frag, beagle_environment.get("app_dir") + "shaders/")


class shader(object):
    def __init__(self,vert,frag, path = None, compile = False ):
        global _reloadables
        if not compile:
            #use shader_load to pull from filesystem
            if path is not None:
                log.write(log.DEBUG,"Compiling USER shader: {0}{1},{2}".format(path,vert,frag))
            else:
                log.write(log.DEBUG,"Compiling ENGINE shader: {0},{1}".format(vert,frag))
            if( path is None):
                vpath =  "shaders/" + vert + ".vert.glsl"
                fpath =  "shaders/" + frag + ".frag.glsl"
            else:
                vpath =  path + vert + ".glsl"
                fpath =  path + frag + ".glsl"

            self._shader = hwgfx.shader_load( vpath, fpath )
            self.vpath = vpath
            self.fpath = fpath
            _reloadables.append( self )
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

    def bind(self,uniforms = [], bind_textures = True, reserved_units = 0 ):
        hwgfx.shader_bind(self._shader);

        if type(uniforms) is dict:
            uniforms = shader.transform_uniforms(uniforms)

        tex_unit = reserved_units

        for u in uniforms:
            name    = u[0]
            vector  = u[1]

            vlen = 0
            if type(vector) is list:
                vlen    = len(vector)

            if vlen == 1:
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
            elif isinstance(vector,texture):
                if bind_textures:
                    vector.bind(tex_unit)
                    tex_unit += 1
                hwgfx.shader_bind_texture( self._shader, name, vector._tex )
            elif isinstance(vector,framebuffer):
                if bind_textures:
                    vector.get_texture().bind(tex_unit)
                    tex_unit += 1
                hwgfx.shader_bind_texture( self._shader, name, vector.get_texture()._tex )
            elif type(vector) is float:
                hwgfx.shader_bind_float (self._shader, name, 
                        vector)

    def __del__(self):
        log.write(log.DEBUG, "Deleting shader program {0}".format(self._shader))
        hwgfx.shader_drop(self._shader)

    @classmethod
    def from_core_library( cls, vert, frag):
        return cls( vert, frag , "shaders/" )

    @classmethod
    def from_application_library( cls, vert, frag):
        return cls( vert, frag, host_config.get("app_dir") + "shaders/" )


