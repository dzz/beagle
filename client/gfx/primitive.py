import client.system.log as log
import itertools
import hwgfx
from client.math.helpers import tesselated_unit_quad, tesselated_unit_quad_uv
from enum import IntEnum


_drawmode_map = []


#bindings as per api-includes/modules/hwgfx.h
class draw_mode(IntEnum):
    POINTS          = 0 
    LINE_STRIP      = 1 
    LINE_LOOP       = 2 
    LINES           = 3 
    TRI_STRIP       = 4 
    TRI_FAN         = 5
    TRIS            = 6

def INIT_set_drawmode_map(hwgfx_map):
    global _drawmode_map
    for mode in hwgfx_map:
        _drawmode_map.append(mode)
        log.write(log.DEBUG, "loaded primitive drawing mode:{0}".format(mode))

class primitive:
    def __init__(self,mode, coords, uvs = None ):
        global _drawmode_map

        self._has_uvs=False

        if(uvs and len(coords) != len(uvs) ):
            raise ValueError("PYPRIM: uvs vertex count must match coords")

        total_coords = len(coords)
        floats_per_vertex = len(coords[0]);
        coords = list(itertools.chain.from_iterable(coords))
        gpu_mode = _drawmode_map[mode]

        #print("PYPRIM: attempt to build {0} vertex primitive".format(total_coords))
        if uvs:
            uvs = list(itertools.chain.from_iterable(uvs));
            if(len(uvs) != total_coords*2):
                log.write(log.ERROR, "primitive uvs don't make sense")
                raise ValueError("uvs dont make sense")

        if(total_coords != len(coords)/floats_per_vertex):
            log.write(log.ERROR, "primitive doesn't make sense")
            raise ValueError("coords array doesn't make sense as vertices")

        if uvs is None:
            self._prim = hwgfx.primitive_create_coordinate_primitive( 
                    coords, 
                    floats_per_vertex,
                    gpu_mode )
            self._has_uvs = False
        else:
            self._prim = hwgfx.primitive_create_coordinate_uv_primitive(
                    coords, uvs,
                    floats_per_vertex,
                    gpu_mode )
            self._has_uvs = True
        log.write(log.DEBUG,"Acquired Primitive {0}".format(self._prim))

    def __del__(self):
        if self._has_uvs:
            hwgfx.primitive_destroy_coordinate_uv_primitive( self._prim )
        else:
            hwgfx.primitive_destroy_coordinate_primitive( self._prim )
        log.write(log.DEBUG,"Dropped Primitive {0}".format(self._prim))

    def render(self):
        hwgfx.primitive_render(self._prim)

    def render_shaded( self, shader_program, shader_inputs = [], bind_textures = True, reserved_units = 0 ):
        shader_program.bind( shader_inputs, bind_textures, reserved_units )
        self.render()
    def get_unit_uv_primitive():
        return primitive( draw_mode.TRIS, tesselated_unit_quad, tesselated_unit_quad_uv )
