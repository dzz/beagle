import json
import os
from client.gfx.tileset         import tileset as tileset_cls
from client.gfx.rect            import rect_tile, rect_tile_start, rect_tile_raw, rect_tile_end
from client.gfx.primitive       import primitive
from client.gfx.primitive       import draw_mode
import client.gfx.texture
import client.beagle.beagle_environment  as beagle_environment
import client.gfx.shaders       as shaders

class tilemap:
    default_shader = shaders.get( "hwgfx/tilemap", "hwgfx/tilemap" )
    def __init__(self, configuration, img_path = None , filtered = False, coordinates = [1.0,1.0], tileheight = None, shader = None, tileset = None ):
        self.coordinates = coordinates
        self.tilesets = []

        if(tileset):
            self.tilesets.append(tileset)

        self.gid_tileset_map = {}  
        self.layers = []
        self.object_layers = {}


        if "extra_channels" in configuration:
            self.extra_channels = configuration["extra_channels"]
        else:
            self.extra_channels = []
 
        self.primitive = None
        self.primaryTileset = None
        if not shader:
            self.shader = tilemap.default_shader
        else:
            self.shader = shader

        if not tileset:
            for tileset_definition in configuration["tilesets"]:
                tileset_definition["extra_channels"] = self.extra_channels
                ts = tileset_cls( tileset_definition, img_path) 
                for gid in range( ts.firstgid, ts.gidcount):
                    self.gid_tileset_map[gid] = ts
                self.tilesets.append(ts)
        else:
           ts = self.tilesets[0]
           for gid in range( ts.firstgid, ts.gidcount):
               self.gid_tileset_map[gid] = ts
        

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

        if(tileheight is None):
            if "tileheight" in configuration:
                self.tileheight_units = configuration["tileheight"]
            else:
                self.tileheight_units = 1.0
        else:
            self.tileheight_units = tileheight

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
                            tx = float(x) - (float(self.tileheight_units) / overlap )
                            ty = float(y) - (float(self.tileheight_units) / overlap )
                            sz = 1.0 + (float(self.tileheight_units)/overlap)

                            tile_coords.extend(   [ [ tx,  ty   ], 
                                                    [ tx+sz,ty   ], 
                                                    [ tx+sz,ty+sz ], 

                                                    [ tx+sz,ty+sz ], 
                                                    [ tx,  ty+sz ], 
                                                    [ tx,  ty   ] ] )

                            eps = 0.0012
                            tile_uvs.extend( [ 
                                               [ tile[0]+eps,         tile[1]+eps         ],
                                               [ tile[0]+tile[2]-eps, tile[1]+eps         ],
                                               [ tile[0]+tile[2]-eps, tile[1]+tile[3]-eps ],
                                               [ tile[0]+tile[2]-eps, tile[1]+tile[3]-eps ],
                                               [ tile[0]+eps        , tile[1]+tile[3]-eps ],
                                               [ tile[0]+eps,         tile[1]+eps         ]
                                               ] )
                        
                    gid_idx+=1


        for coord in tile_coords:
            coord[0] = (coord[0] * self.tileheight_units) - (layer["width"] * self.tileheight_units * 0.5)
            coord[1] = (coord[1] * self.tileheight_units) - (layer["height"] * self.tileheight_units * 0.5)



        if(len(tile_coords)>0):
            self.primitive = primitive( draw_mode.TRIS, tile_coords, tile_uvs )
        else:
            self.primitive = None
          

        
    def pixel_to_units(self, coord):
        coord[0] = (coord[0]/(self.primaryTileset.tileheight / self.tileheight_units)) - (self.layers[0]['width'] * self.tileheight_units * 0.5)
        coord[1] = (coord[1]/(self.primaryTileset.tileheight / self.tileheight_units)) - (self.layers[0]['height'] * self.tileheight_units * 0.5)
        return coord

    def pixelsize_to_unitsize(self, size ):
        size[0] = (size[0]/(self.primaryTileset.tileheight / self.tileheight_units))
        size[1] = (size[1]/(self.primaryTileset.tileheight / self.tileheight_units))
        return size

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

        if(self.primitive):
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
    
    def set_view(self,view):
        self.coordinates = view

    @classmethod 
    def from_json_file(cls, path, img_path, filtered=False, coordinates = [1,1], tileheight = None, extra_channels = [] ):
        root = beagle_environment.get_config("app_dir")
        if not root in path and not os.path.isabs(path):
            path = "{0}{1}".format(root,path)

        json_parsed = {}
        with open(path) as f:
            json_data = f.read()
            json_parsed = json.loads(json_data)
            json_parsed["extra_channels"] = extra_channels
        return cls(json_parsed, img_path, filtered, coordinates, tileheight )

