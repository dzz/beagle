from client.beagle.beagle_api import api as BGL
from .Object import Object

def tiledcolor_convert(colorstring):
    colorstring = colorstring[3:]
    r, g, b = colorstring[:2], colorstring[2:4], colorstring[4:]
    r, g, b = [int(n, 16) for n in (r, g, b)]
    return [r/256.0, g/256.0, b/256.0, 1.0]

class Tilemap():
    def __init__(self,**kwargs):
        BGL.auto_configurable.__init__(self,
            {
                "beagle_tilemap" : BGL.assets.get("NL-placeholder/tilemap/simple_game_room"),
                "color" : [1.0,1.0,1.0,1.0],
                "top_left" : [ -0, -0 ],
                "light_texture_namespace" : "NL-lights"
            }, **kwargs )
        self.shader = BGL.assets.get("beagle-nl/shader/tilemap")
    
    def normalize_relative_coordinate( self, td_object, coord ):
        x = (float( (td_object['x'] + coord[0])) / 8.0);
        y = (float( (td_object['y'] + coord[1])) / 8.0);
        return [x,y]

    def normalize_coordinate( self, td_object, coord ):
        x = (float( (td_object['x'] + coord[0])) ) + self.top_left[0];
        y = (float( (td_object['y'] + coord[1])) ) + self.top_left[1];

        return self.beagle_tilemap.pixel_to_units( [x,y] )

    def get_photon_emitters(self):
        emitters = []
        if "photon_emitters" in self.beagle_tilemap.object_layers:
            for emitter in self.beagle_tilemap.object_layers["photon_emitters"]:
                coord = self.beagle_tilemap.pixel_to_units( [ emitter["x"], emitter["y"] ] )
                size = self.beagle_tilemap.pixelsize_to_unitsize( [ emitter["width"], emitter["height"] ] )
                if "emission_color" in emitter["properties"]:
                    color = tiledcolor_convert( emitter["properties"]["emission_color"])
                else:
                    color = [ 1.0,1.0,1.0,1.0 ]
                emitters.append([coord[0], coord[1], size[0], size[1], color])
        return emitters
                
    def get_light_objects(self):
        light_objects = []
        if "lights" in self.beagle_tilemap.object_layers:
            for light in self.beagle_tilemap.object_layers['lights']:
                coord = self.normalize_coordinate( light, [ light['x'], light['y'] ] )
                size = self.normalize_relative_coordinate( light, [ light['width'], light['height'] ] )
                light_type = None

                if "light_texture" in light["properties"]:
                    light_texture = BGL.assets.get( self.light_texture_namespace + "/texture/" + light['properties']['light_texture'] )
                else:
                    light_texture = None

                if light["properties"]["casts_shadow"]:
                    if light["properties"]["dynamic"]:
                        light_type = Object.LightTypes.DYNAMIC_SHADOWCASTER
                    else:
                        light_type = Object.LightTypes.STATIC_SHADOWCASTER
                else:
                    if light["properties"]["dynamic"]:
                        light_type = Object.LightTypes.DYNAMIC_TEXTURE_OVERLAY
                    else:
                        light_type = Object.LightTypes.STATIC_TEXTURE_OVERLAY

                light_radius = (size[0]+size[1])*0.5;
                 
                light_color = tiledcolor_convert( light["properties"]["light_color"])
                light_object = Object( visible = False, light_type = light_type, p = coord, size=size, light_radius = light_radius, texture = light_texture, color = light_color )
                light_objects.append(light_object)
        return light_objects
                

    def get_light_occluders(self):
        """ convert polyline objects loaded from the the tilemap into occluder format.
            requires that the tilemap have a 'light_occluders' object layer """
        occluders = []
        if "light_occluders" in self.beagle_tilemap.object_layers:
            for lo_object in self.beagle_tilemap.object_layers["light_occluders"]:
                if "polyline" in lo_object:
                    for idx in range(0, len(lo_object["polyline"])-1):
                        base_coord_a = [ lo_object["polyline"][idx]['x'], lo_object["polyline"][idx]['y'] ]
                        base_coord_b = [ lo_object["polyline"][idx+1]['x'], lo_object["polyline"][idx+1]['y'] ]
                        occluders.append([
                            self.normalize_coordinate(lo_object, base_coord_a),
                            self.normalize_coordinate(lo_object, base_coord_b)
                        ]);
        return occluders
            
    def linkFloor( self, floor ):
        self.floor = floor
        self.floor.objects.extend( self.get_light_objects() ) 

    def get_camera(self):
        return self.floor.camera

    def render(self, channel = None):
        camera = self.floor.camera
        self.beagle_tilemap.primitive.render_shaded( self.shader, {
            "tileset" : self.beagle_tilemap.primaryTileset.texture,
            "translation_local"    : [ 0, 0 ],
            "scale_local"          : [ 1.0, 1.0],
            "translation_world"    : self.get_camera().translate_position( [0.0,0.0] ),
            "scale_world"          : self.get_camera().get_scale(),
            "view"                 : self.get_camera().get_view(),
            "rotation_local"       : 0.0
        } );
        ## [ org_x , org_y ] = camera.translate_position( self.top_left )
        ## self.beagle_tilemap.set_view( self.floor.camera.get_view() )

        ## self.beagle_tilemap.render( org_x, org_y, camera.get_zoom(), channel, self.shader )
            
