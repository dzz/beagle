
### 
### ... well at least it works...
###

from random import uniform
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
from client.gfx.tilemap import tilemap
import client.gfx.shaders as shaders
from client.gfx.framebuffer import framebuffer as fb_class
from client.gfx.coordinates import centered_view,Y_Axis_Down, Y_Axis_Up
from client.gfx.primitive import primitive
from client.gfx.blend            import blendstate,blendmode
import os
import os.path
import audio

def get_real_asset_path(relpath):
         
        dirpath = os.getcwd().replace("\\","/")
        try:
            dirpath = beagle_environment.get_config("app_dir")
        except KeyError:
            pass

        path = os.path.join( dirpath, relpath )

        print("LOAD REQ",path)
        return path

class resource_manager:
        instance = None
        def __init__(self, config):

            resource_manager.instance = self

            self.package_keys = {}
            self.resource_map = {}
            self.loaded_packages = []

            self.package_paths = config["package_paths"]
            self.package_data = config["packages"]
            self.adapters = { 
                              "animation" : animation_adapter,
                              "texture"     : tex_adapter,
                              "tileset"     : tileset_adapter,
                              "tilemap"      : tilemap_adapter,
                              "audio_clip"  : audioclip_adapter,
                              "shader"      : shader_adapter,
                              "coordsys"    : coordsys_adapter,
                              "dict"        : dict_adapter,
                              "curve_sequence" : scene_adapter,
                              "curve" : curve_adapter,
                              "default" : default_adapter,
                              "textfile" : textfile_adapter,
                              "path" : path_adapter
                              }


            self.load_specials()

            for pkg in self.package_data:
                pkg_def = self.package_data[pkg]
                self.package_keys[pkg] = []
                if(pkg_def["preload"]):
                    self.load_package(pkg)

        def replace_paths(self, string, pkg_path, shader = False ):
    
            if string[0] is "/":
                string = beagle_environment.get_config("app_dir") + string

            if not "{" in string and shader:
                string = "shaders/" + string

            parsed = string
            parsed = parsed.replace("{package_path}", os.path.join(pkg_path))
            for key in self.package_paths:
                repl = "{" + key + "}"
                parsed = parsed.replace(repl,os.path.join(self.package_paths[key]))

            if(parsed  == string):
                parsed = get_real_asset_path(string)
        

            return parsed
            
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
            pkg_path = self.package_paths[pkgname]

            if type(pkg_def["resources"]) is list:
                for resource_definition in pkg_def["resources"]:
                    self.load_resource(pkgname, resource_definition, pkg_path)
            if type(pkg_def["resources"]) is dict:
                for typekey in pkg_def["resources"]:
                    for resource_definition in pkg_def["resources"][typekey]:
                        resource_definition["type"] = typekey
                        self.load_resource(pkgname, resource_definition, pkg_path)

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
                #log.write( log.INFO, "Flushed asset {0} from package {1}".format(key,pkgname))
            for key in rm_keys:
                del self.resource_map[key]
            self.loaded_packages.remove(pkgname)
            #log.write( log.INFO, "Flushed package {0}".format(pkgname) )

        def load_resource(self, pkgname, resdef, fs_path):

            print(json.dumps(resdef))
            log.write( log.DEBUG, "Loading:" + fs_path )
            if resdef["type"] in self.adapters:
                adapter = self.adapters[resdef["type"]]
            else:
                adapter = self.adapters["default"]                

            key = "{0}/{1}/{2}".format(pkgname, resdef["type"], resdef["name"])
            self.package_keys[pkgname].append(key)
            self.resource_map[key] = adapter.load(resdef, key, fs_path)
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
                #log.write(log.DEBUG, "Flushed asset {0}".format(key))
            for key in rm_keys:
                del self.resource_map[key]


class tex_adapter:
    def load(tex_def, key, pkg_path):
        imagename = resource_manager.instance.replace_paths( tex_def["filename"], pkg_path )
        tex = texture.from_local_image( local_image.from_file(imagename), tex_def["filtered"])
        tex.debugger_attach(key)
        return tex

import re

def natural_sort(l): 
    convert = lambda text: int(text) if text.isdigit() else text.lower() 
    alphanum_key = lambda key: [ convert(c) for c in re.split('([0-9]+)', key) ] 
    return sorted(l, key = alphanum_key)

class animation_adapter:
    def load(tex_def, key, pkg_path):
        convert = lambda text: int(text) if text.isdigit() else text.lower() 
        dirname = resource_manager.instance.replace_paths( tex_def["directory"], pkg_path )
        ret = []

        files =  natural_sort(os.listdir(dirname))
        
        #print(files)

        for f in files:
            fname = os.path.join(dirname , f)
            tex = texture.from_local_image( local_image.from_file(fname), tex_def["filtered"])
            tex.animation_id = os.path.basename( fname )
            #tex._bgl_assets_sortkey = lambda key: [ convert(c) for c in re.split('([0-9]+)', os.path.splitext(f)[0]) ] 

#'{0:0>8}'.format(os.path.splitext(f)[0]).lower()

            #if "debug" in tex_def and tex_def["debug"]:
            #    print(tex._bgl_assets_sortkey)
            ret.append(tex)

        #ret.sort(key=lambda x: x._bgl_assets_sortkey)

        if "reverse" in tex_def and tex_def["reverse"]:
            ret.reverse()
        #if "debug" in tex_def and tex_def["debug"]:
        #    exit()

        return ret

class tileset_adapter:
    def load(ts_def, key, pkg_path):
        return tileset( ts_def, "", ts_def["filtered"] ) 

class tilemap_adapter:
    def load(tm_def, key, pkg_path):
        fn = resource_manager.instance.replace_paths( tm_def["json_file"], pkg_path )
        tm_def["tileset_directory"] = resource_manager.instance.replace_paths( tm_def["tileset_directory"], pkg_path )

        if tm_def["tileheight"]:
            tileheight = tm_def["tileheight"]
        else:
            tileheight = None
        return tilemap.from_json_file( fn, tm_def["tileset_directory"], tm_def["filtered"], tm_def["coordinates"], tm_def["tileheight"], tm_def["extra_channels"])

class audioclip_adapter:
    def load(ac_def, key, pkg_path):
        fn = resource_manager.instance.replace_paths( ac_def["filename"] )
        return audio.clip_create(fn)

class coordsys_adapter:
    def load(cs_def, key, pkg_path):
        if cs_def["mode"] == "centered_view":
            if cs_def["y_axis"] == "down":
                y_axis = Y_Axis_Down
            if cs_def["y_axis"] == "up":
                y_axis = Y_Axis_Up

            return centered_view(cs_def["width"],cs_def["height"], y_axis )

class default_adapter:
    def load(definition, key, pkg_path):
        return definition

class textfile_adapter:
    def load(dict_def, key, pkg_path):
        filepath = resource_manager.instance.replace_paths(dict_def["filename"], pkg_path, True)
        file = open(filepath)
        return file.read()

class path_adapter:
    def load(dict_def, key, pkg_path):
        filepath = resource_manager.instance.replace_paths(dict_def["path"], pkg_path, True)
        return filepath
     
class dict_adapter:
    def load(dict_def, key, pkg_path):
            return dict_def["dict"]

class curve_adapter:
    def load(dict_def, key, pkg_path):
        return curve( dict_def["points"] )

class scene_adapter:
    def load(dict_def, key, pkg_path):
            return curve_sequencer( dict_def["sequence"] )

class shader_adapter:
    def load(shd_def, key, pkg_path):

        shd_def["vertex_program"] = resource_manager.instance.replace_paths( shd_def["vertex_program"], pkg_path , True )
        shd_def["pixel_program"] = resource_manager.instance.replace_paths( shd_def["pixel_program"], pkg_path , True)

        if "unique_instances" in shd_def and shd_def["unique_instances"]:
            return shaders.get_unique_client_program( shd_def["vertex_program"], shd_def["pixel_program"],"" )
        else:
            return shaders.get_client_program( shd_def["vertex_program"], shd_def["pixel_program"],"" )


class assets:
        def register_adapter( key, adapter_class ):
            resource_manager.instance.adapters[key] = adapter_class

        def get(path):
            return resource_manager.instance.get_resource(path)

        def exec(path, arguments = [] ):
            return resource_manager.instance.get_resource(path)(*arguments)

        def _xfkey(path,k):
            return "{0}/reusables/" + k

        def lazy_reusable(factory,args,key):
            return assets.reusable_from_factory( factory,args,key)

        def reusable_from_factory( factory, args, key):
            key = assets._xfkey(factory, key)
            if key not in resource_manager.instance.resource_map:
                resource_manager.instance.resource_map[key] = assets.exec( factory, args)
            return resource_manager.instance.resource_map[key]

        def flush_reusable( factory, key ):
            key = assets._xfkey(path, key)
            if( key in resource_manager.instance.resource_map ):
                del resource_manager.instance.resource_map[key]

        def exec_range(path, arg_collection):
            r = []
            for arg_list in arg_collection:
                r.append(assets.exec(path,arg_list))
            return r

        def load_packages(pkgname):
            assets.load_package(pkgname)

        def load_package(pkgname):
            if type(pkgname) is list:
                for i_pkgname in pkgname:
                    resource_manager.instance.load_package(i_pkgname)
            else:
                resource_manager.instance.load_package(pkgname)

        def flush_package(pkgname):
            resource_manager.instance.flush_package(pkgname)

class asset_manager:
        def get(path):
            return resource_manager.instance.get_resource(path)
    
        def compile(master_manifest_path):

            cwd = os.getcwd().replace("\\","/")
            if master_manifest_path is None:
                package_map = { "packages":{}, "package_paths":{} }

                with open(os.path.join( cwd, "shaders/beagle-2d/beagle-2d.json"),'r') as package_file:
                    package_map["packages"]["beagle-2d"] = json.load(package_file)
                    package_map["package_paths"]["beagle-2d"] = os.path.dirname(os.path.join(cwd,"shaders/beagle-2d/"))

                with open(os.path.join( cwd, "shaders/beagle-2d/beagle-2d.json"),'r') as package_file:
                    package_map["packages"]["beagle-nl"] = json.load(open(os.path.join( cwd, "shaders/beagle-nl/beagle-nl.json"),'r'))
                    package_map["package_paths"]["beagle-nl"] = os.path.dirname(os.path.join(cwd,"shaders/beagle-nl/"))

                resource_manager.instance = resource_manager(package_map)
                return


            filepath = get_real_asset_path(master_manifest_path)
            with open(filepath, "r") as master_manifest_file:
                    master_manifest_data = { "packages" : {} }

                    master_manifest_data["packages"]["beagle-2d"] = os.path.join( cwd, "shaders/beagle-2d/beagle-2d.json")
                    master_manifest_data["packages"]["beagle-nl"] = os.path.join( cwd, "shaders/beagle-nl/beagle-nl.json")
                    master_manifest_data["packages"]["NL-placeholder"] = os.path.join( cwd, "placeholder_resources/placeholder/placeholder.json")
                    master_manifest_data["packages"]["NL-camera"] = os.path.join( cwd, "placeholder_resources/camera.json")
                    master_manifest_data["packages"]["NL-lights"] = os.path.join( cwd, "placeholder_resources/lights/lights.json")

                    application_manifest_data = json.load(master_manifest_file)
                    parsed_manifest_data = { "packages" : {}, "package_paths" : {} }

                    for manifest_data in [ master_manifest_data, application_manifest_data ]:
                        for package_alias in manifest_data["packages"]:
                            package_manifest_path = manifest_data["packages"][package_alias]

                            if(manifest_data is application_manifest_data):
                                absolute_package_manifest_path = get_real_asset_path(package_manifest_path)
                            else:
                                absolute_package_manifest_path = package_manifest_path

                            with open( absolute_package_manifest_path,"r") as package_file:
                                try:
                                    package_manifest_data = json.load(package_file)
                                except Exception as e:
                                        print("Could not compile package{0}".format(absolute_package_manifest_path))
                                        raise e
                                parsed_manifest_data["packages"][package_alias] = package_manifest_data
                                parsed_manifest_data["package_paths"][package_alias] = os.path.dirname(os.path.abspath( absolute_package_manifest_path )).replace("\\","/")

                    resource_manager.instance = resource_manager(parsed_manifest_data) 

