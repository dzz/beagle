import json
from client.gfx.context import gfx_context
from client.gfx.text import render_text
from client.system.gamepad       import get_gamepad, pad_buttons
import client.system.log as log
from client.math.curve import curve
from client.beagle.animation import curve_sequencer
from client.gfx.texture import texture
import client.beagle.beagle_environment  as beagle_environment
from client.gfx.local_image import local_image
from client.gfx.tileset import tileset
import client.gfx.shaders as shaders
from client.gfx.framebuffer import framebuffer as fb_class
from client.gfx.coordinates import centered_view,Y_Axis_Down, Y_Axis_Up
from client.gfx.primitive import primitive
from client.gfx.blend            import blendstate,blendmode
import os
import audio


def cvt_path(relpath):
        r = beagle_environment.get_config("app_dir") + relpath
        return r

class resource_manager:
        def __init__(self, config):
            self.package_keys = {}
            self.resource_map = {}
            self.loaded_packages = []
            self.package_data = config["packages"]
            self.adapters = { "texture"     : tex_adapter,
                              "tileset"     : tileset_adapter,
                              "audio_clip"  : audioclip_adapter,
                              "shader"      : shader_adapter,
                              "coordsys"    : coordsys_adapter,
                              "dict"        : dict_adapter,
                              "curve_sequence" : scene_adapter,
                              "curve" : curve_adapter,
                              "default" : default_adapter
                              }


            self.load_specials()

            for pkg in self.package_data:
                pkg_def = self.package_data[pkg]
                self.package_keys[pkg] = []
                if(pkg_def["preload"]):
                    self.load_package(pkg)



        def load_specials(self):
            def find_primary_gamepad():
                return get_gamepad(0)

            def print_pixels(txt,pos,col):
                return render_text(txt,pos[0],pos[1],col)

            def print_rows(txt,pos,col):
                return print_pixels(txt,[pos[0]*8,pos[1]*8],col)

            def scroll_text(strg,idx):
                idx = idx % len(strg)
                r = ""
                for i in range(0,len(strg)):
                    r = r + strg[(i+idx)%len(strg)]
                return r

            self.resource_map["core/primitive/unit_uv_square"] = primitive.get_unit_uv_primitive()
            self.resource_map["core/factory/framebuffer/from_dimensions[w,h]"] = fb_class.from_dims
            self.resource_map["core/factory/framebuffer/[w,h]"] = fb_class.from_dims
            self.resource_map["core/factory/framebuffer/from_screen"] = fb_class.from_screen
            self.resource_map["core/queries/gamepad/find_by_id[id]"] = get_gamepad
            self.resource_map["core/queries/gamepad/find_primary"] = find_primary_gamepad
            self.resource_map["core/gamepad/buttons"] = pad_buttons
            self.resource_map["core/stringfx/scroll[txt,offset]"] = scroll_text
            self.resource_map["core/lotext/print(pixels)[txt,x,y,[r,g,b]]"] = render_text
            self.resource_map["core/lotext/print(pixels)[txt,[x,y],[r,g,b]]"] = print_pixels
            self.resource_map["core/lotext/print[txt,[x,y],[r,g,b]]"] = print_pixels
            self.resource_map["core/lotext/print(rows)[txt,[x,y],[r,g,b]]"] = print_rows
            self.resource_map["core/hwgfx/context/clear[r,g,b,a]"] = gfx_context.clear_rgba
            self.resource_map["core/hwgfx/blendmode/alpha_over"] = blendstate( blendmode.alpha_over )
            self.resource_map["core/hwgfx/blendmode/add"] = blendstate( blendmode.add )
            self.resource_map["core/hwgfx/primitive/unit_uv_square"] = primitive.get_unit_uv_primitive()
        

        def load_package(self,pkgname):
            if pkgname in self.loaded_packages:
                return
            pkg_def = self.package_data[pkgname]

            if type(pkg_def["resources"]) is list:
                for resource_definition in pkg_def["resources"]:
                    self.load_resource(pkgname, resource_definition)
            if type(pkg_def["resources"]) is dict:
                for typekey in pkg_def["resources"]:
                    for resource_definition in pkg_def["resources"][typekey]:
                        resource_definition["type"] = typekey
                        self.load_resource(pkgname, resource_definition)

            self.loaded_packages.append(pkgname)
            log.write( log.INFO, "Loaded asset package:{0}".format(pkgname))

        def flush_package(self,pkgname):
            if pkgname not in self.loaded_packages:
                raise ValueError("tried to flush {0} package which was not loaded".format(pkgname))
            flush_keys = self.package_keys[pkgname]
            rm_keys = []
            for key in flush_keys:
                self.resource_map[key] = None
                rm_keys.append(key)
                log.write( log.INFO, "Flushed asset {0} from package {1}".format(key,pkgname))
            for key in rm_keys:
                del self.resource_map[key]
            self.loaded_packages.remove(pkgname)
            log.write( log.INFO, "Flushed package {0}".format(pkgname) )

        def load_resource(self, pkgname, resdef):
            if resdef["type"] in self.adapters:
                adapter = self.adapters[resdef["type"]]
            else:
                adapter = self.adapters["default"]                

            key = "{0}/{1}/{2}".format(pkgname, resdef["type"], resdef["name"])
            self.package_keys[pkgname].append(key)
            self.resource_map[key] = adapter.load(resdef)
            log.write( log.DEBUG, "Loaded asset {0}".format(key))

        def get_resource(self, path):
            try:
                return self.resource_map[path]
            except KeyError:
                log.write( log.ERROR, "Could not load asset {0}".format(path))
                return None

        def __del__(self):
            rm_keys = []
            for key in self.resource_map:
                self.resource_map[key] = None
                rm_keys.append(key)
                log.write(log.DEBUG, "Flushed asset {0}".format(key))
            for key in rm_keys:
                del self.resource_map[key]


class tex_adapter:
    def load(tex_def):
        imagename = cvt_path(tex_def["filename"])
        return texture.from_local_image( local_image.from_file(imagename), tex_def["filtered"])

class tileset_adapter:
    def load(ts_def):
        return tileset( ts_def, "", ts_def["filtered"] ) 


class audioclip_adapter:
    def load(ac_def):
        return audio.clip_create(beagle_environment.get("app_dir") + ac_def["filename"])

class coordsys_adapter:
    def load(cs_def):
        if cs_def["mode"] == "centered_view":
            if cs_def["y_axis"] == "down":
                y_axis = Y_Axis_Down
            if cs_def["y_axis"] == "up":
                y_axis = Y_Axis_Up

            return centered_view(cs_def["width"],cs_def["height"], y_axis )

class default_adapter:
    def load(definition):
        return definition
 
class dict_adapter:
    def load(dict_def):
            return dict_def["dict"]

class curve_adapter:
    def load(dict_def):
        return curve( dict_def["points"] )

class scene_adapter:
    def load(dict_def):
            return curve_sequencer( dict_def["sequence"] )

class shader_adapter:
    def load(shd_def):
        return shaders.get_client_program( shd_def["vertex_program"], shd_def["pixel_program"] )

instance = None
class assets:
        def register_adapter( key, adapter_class ):
            global instance
            instance.adapters[key] = adapter_class

        def get(path):
            global instance
            return instance.get_resource(path)

        def exec(path, arguments = [] ):
            global instance
            return instance.get_resource(path)(*arguments)

        def _xfkey(path,k):
            return "{0}/reusables/" + k

        def lazy_reusable(factory,args,key):
            return assets.reusable_from_factory( factory,args,key)

        def reusable_from_factory( factory, args, key):
            global instance
            key = assets._xfkey(factory, key)
            if key not in instance.resource_map:
                instance.resource_map[key] = assets.exec( factory, args)
            return instance.resource_map[key]

        def flush_reusable( factory, key ):
            global instance
            key = assets._xfkey(path, key)
            if( key in instance.resource_map ):
                del instance.resource_map[key]

        def exec_range(path, arg_collection):
            r = []
            for arg_list in arg_collection:
                r.append(assets.exec(path,arg_list))
            return r

        def load_packages(pkgname):
            assets.load_package(pkgname)

        def load_package(pkgname):
            global instance
            if type(pkgname) is list:
                for i_pkgname in pkgname:
                    instance.load_package(i_pkgname)
            else:
                instance.load_package(pkgname)

        def flush_package(pkgname):
            global instance
            return instance.flush_package(pkgname)

class asset_manager:


        def get(path):
            global instance
            return instance.get_resource(path)
    

        def compile(json_file):
            path = cvt_path(json_file)
            with open(path, "r") as resources_file:
                    data = json.load(resources_file)
                    for pkg_key in data["packages"]:
                        pkg_val = data["packages"][pkg_key]
                        if type(pkg_val) is str:
                            with open( cvt_path(pkg_val),"r") as package_file:
                                print(pkg_val)
                                pkg_data = json.load(package_file)
                                data["packages"][pkg_key] = pkg_data
                    global instance
                    instance = resource_manager(data) 

