import json
from client.gfx.tileset         import tileset
from client.gfx.rect            import rect_tile, rect_tile_start, rect_tile_raw, rect_tile_end
from client.gfx.primitive       import primitive
from client.gfx.primitive       import draw_mode
import client.gfx.texture
import client.beagle.beagle_environment  as beagle_environment
import client.gfx.shaders       as shaders

class tilemap:
    def __init__(self, configuration, img_path, filtered = False, coordinates = [1.0,1.0], tileheight = None, shader = None ):
        self.coordinates = coordinates
        self.tilesets = []
        self.gid_tileset_map = {}  
        self.layers = []
        self.object_layers = {}

        if(tileheight is None):
            self.tileheight = configuration["tileheight"]
        else:
            self.tileheight = tileheight

        if "extra_channels" in configuration:
            self.extra_channels = configuration["extra_channels"]
        else:
            self.extra_channels = []
 
        self.primitive = None
        self.primaryTileset = None
        if not shader:
            self.shader = shaders.get( "hwgfx/tilemap", "hwgfx/tilemap" )
        else:
            self.shader = shader

        for tileset_definition in configuration["tilesets"]:
            tileset_definition["extra_channels"] = self.extra_channels
            ts = tileset( tileset_definition, img_path) 
            for gid in range( ts.firstgid, ts.gidcount):
                self.gid_tileset_map[gid] = ts
            self.tilesets.append(ts)

        for layer_definition in configuration["layers"]:
            if "data" in layer_definition:
                layer = {}
                layer["height"] = layer_definition["height"]
                layer["width"] = layer_definition["width"]
                layer["data"] = layer_definition["data"]
                self.layers.append(layer)
            if "objects" in layer_definition:
                self.object_layers[layer_definition["name"]] = layer_definition["objects"] 


        if(len(self.tilesets)>0):
             self.primaryTileset = self.tilesets[0]
        else:
             raise ValueError("Input JSON for tilemap must have at least one tileset")

        self.compile()

    def get_polylines(self,name):
        return self.polyline_layers[name]

    def set_coordinates(self,coordinates):
        self.coordinates = coordinates

    def get_layer_tile(self,layer_index,x,y):
        try:
            x = int(x)
            y = int(y)
            layer = self.layers[layer_index]
            index = int( layer["width"]*y+x )
            return layer["data"][index]
        except IndexError:
            return None

    def compile(self):
        tile_coords = []
        tile_uvs    = []

        for layer in self.layers:
            rows = range(0,layer["height"])
            columns = range(0,layer["width"])
            layer_data = layer["data"]

            gid_idx = 0
            for y in rows:
                for x in columns:
                    gid_id = layer_data[gid_idx]
                    if(gid_id>0):
                        ts = self.gid_tileset_map[gid_id]
                        tile = ts.get_gid(gid_id)
                        if(tile):
                            overlap = 500
                            tx = float(x) - (float(self.tileheight) / overlap )
                            ty = float(y) - (float(self.tileheight) / overlap )
                            sz = 1.0 + (float(self.tileheight)/overlap)

                            tile_coords.extend(   [ [ tx,  ty   ], 
                                                    [ tx+sz,ty   ], 
                                                    [ tx+sz,ty+sz ], 

                                                    [ tx+sz,ty+sz ], 
                                                    [ tx,  ty+sz ], 
                                                    [ tx,  ty   ] ] )

                            tile_uvs.extend( [ 
                                               [ tile[0],         tile[1]         ],
                                               [ tile[0]+tile[2], tile[1]         ],
                                               [ tile[0]+tile[2], tile[1]+tile[3] ],
                                               [ tile[0]+tile[2], tile[1]+tile[3] ],
                                               [ tile[0]        , tile[1]+tile[3] ],
                                               [ tile[0],         tile[1]         ]
                                               ] )
                        
                    gid_idx+=1


        for coord in tile_coords:
            coord[0] = (coord[0] * self.tileheight) - (layer["width"])
            coord[1] = (coord[1] * self.tileheight) - (layer["height"])


        self.primitive = primitive( draw_mode.TRIS, tile_coords, tile_uvs )
          

        
    def pixel_to_units(self, coord):
        coord[0] = (coord[0]/8.0) - self.layers[0]['width']
        coord[1] = (coord[1]/8.0) - self.layers[0]['height']
        return coord

    def set_view( self, view ):
        self.coordinates = view

    def render(self,org_x,org_y,scale, channel = None, custom_shader = None ):
        if custom_shader is None:
            shader = self.shader
        else:
            shader = custom_shader

        if channel is None:
            texture = self.primaryTileset.texture
        else:
            texture = self.primaryTileset.channel_textures[channel]

        shader.bind({
            "tileset" : texture,
            "scale" : [ scale ],
            "view" : self.coordinates,
            "translation" : [float(org_x),float(org_y)]
        })
        self.primitive.render()
        return

    def gid_via_coord(self,x,y,layer):
        i = x+(y*self.layers[layer]["width"])
        gid_id = self.layers[layer]["data"][i]
        return gid_id

    def tile_prop_via_coord(self,x,y,layer,key):
        i = x+(y*self.layers[layer]["width"])
        gid_id = self.layers[layer]["data"][i]
        ts = self.gid_tileset_map[gid_id]
        return ts.tile_prop(gid_id,key)

    @classmethod 
    def from_json_file(cls, path, img_path, filtered=False, coordinates = [1,1], tileheight = None, extra_channels = [] ):
        root = beagle_environment.get_config("app_dir")
        if not root in path:
            path = "{0}{1}".format(root,path)

        json_parsed = {}
        with open(path) as f:
            json_data = f.read()
            json_parsed = json.loads(json_data)
            json_parsed["extra_channels"] = extra_channels
        return cls(json_parsed, img_path, filtered, coordinates, tileheight )

