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
                "tilemap" : BGL.assets.get("NL-placeholder/tilemap/simple_game_room"),
                "color" : [1.0,1.0,1.0,1.0],
                "top_left" : [ -48 , -48 ],
                "light_texture_namespace" : "NL-lights"
            }, **kwargs )

    
    def normalize_relative_coordinate( self, td_object, coord ):
        x = (float( (td_object['x'] + coord[0])) / 8.0);
        y = (float( (td_object['y'] + coord[1])) / 8.0);
        return [x,y]

    def normalize_coordinate( self, td_object, coord ):
        x = (float( (td_object['x'] + coord[0])) / 8.0) + self.top_left[0];
        y = (float( (td_object['y'] + coord[1])) / 8.0) + self.top_left[1];
        return [x,y]

    def get_light_objects(self):
        light_objects = []
        if "lights" in self.tilemap.object_layers:
            for light in self.tilemap.object_layers['lights']:
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
        if "light_occluders" in self.tilemap.object_layers:
            for lo_object in self.tilemap.object_layers["light_occluders"]:
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

    def render(self, channel = None):

        camera = self.floor.camera

        [ org_x , org_y ] = camera.translate_position( self.top_left )

        self.tilemap.set_view( self.floor.camera.get_view() )
        if channel is None:
            self.tilemap.render( org_x, org_y, camera.get_zoom() )
        else:
            self.tilemap.render( org_x, org_y, camera.get_zoom(), channel )
            
