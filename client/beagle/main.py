import traceback
import sys
import beagle_runtime
import hwgfx
import configparser
import client.system.log as log
import client.system.gamepad as gamepad
import client.system.keyboard as keyboard

from client.beagle.mouse_focus import mouse_focused_area
from client.beagle.status import render_status
from client.beagle.status import set_status

import client.beagle.beagle_environment  as beagle_environment
import client.apps
import client.ui.areas as ui_area
import client.gfx.blend as blend
import client.gfx.primitive as primitive
import client.gfx.shaders as shaders
from client.gfx.framebuffer import framebuffer,render_target
import client.beagle.caret as caret
from   client.beagle.assets import asset_manager
import gc
import os

from client.system.telnet_console import *
from client.system.http_server import bgl_http_server

from client.system.console import console as gui_console
from client.beagle.beagle_engine import beagle_engine

from time import sleep


class frames:
    A = framebuffer.from_screen()
    B = framebuffer.from_screen()
    render = None
    display = None

    shader = shaders.shader("beagle-2d/vertex/no_transform", "beagle-2d/pixel/passthru","shaders/")
    def swap_render():
        if(frames.render == frames.A):
            frames.render = frames.B
        else:
            frames.render = frames.A

    def swap_display():
        if(frames.display == frames.A):
            frames.display = frames.B
        else:
            frames.display = frames.A

    def render_display():
        frames.display.render_processed( frames.shader )
        #frames.swap_display()



frames.render = frames.A
frames.display = frames.B


__clickpos  = [0,0]
__mpos      = [0,0]

console = None
http_server = None

global app

def beagle_halt(e):
    print(e)
    traceback.print_tb(e.__traceback__)
    os._exit(1)


def init():
    def bool(v):
        if v is False:
            return False
        return v.lower() in ("true","1")

    global app, console, http_server

    class output_redirect():
        def cvted(txt):
            lines = txt.split("\n")
            cvt_lines = []
            for line in lines:
                if(len(line) > 0):
                    cvt_lines.append(line.rstrip())
            return cvt_lines

        def flush(self):
            return

        def _wr(txt, mode):
            for line in output_redirect.cvted(txt):
                log.write(mode, line )

        def write(self,txt):
            output_redirect._wr(txt,log.INFO)

    class error_redirect():
        def flush(self):
            return

        def write(self,txt):
            output_redirect._wr(txt,log.ERROR)

    #sys.stdout = output_redirect()
    #sys.stderr = error_redirect()

    config = configparser.ConfigParser()

    keyboard.register_keyup_handler('backquote', shaders.reload )
    keyboard.register_keyup_handler(gui_console.toggle_key, gui_console.toggle )

    if(beagle_runtime.get_user_specified_application_folder()):
        target_application_folder = os.path.normpath(beagle_runtime.get_user_specified_application_folder())
        log.write(log.INFO, "Loading application @:{0}".format(target_application_folder))
    else:
        target_application_folder = None

    loading_external = False

    if(os.path.isfile("client/generated_application.ini")):
        config.read("client/generated_application.ini")
    else:
        if target_application_folder is None:
            config.read("client/application.ini")
        else:
            loading_external = True
            ini_path = os.path.join( target_application_folder, "application.ini")
            print(ini_path)
            config.read(ini_path)

    print(config["APPLICATION"])
    app_name = config["APPLICATION"]["name"]
    beagle_runtime.set_title(app_name)
    try:
        app_dir = config["APPLICATION"]["path"]
        log.write(log.INFO, "Loading application @:{0}".format(app_dir))
        app_module = config["APPLICATION"]["module"]
    except:
        app_dir = os.path.dirname( target_application_folder )
        app_module = os.path.basename( target_application_folder )
    try:
        resource_json = config["APPLICATION"]["assets"]
    except:
        resource_json = None


    controller_enabled = bool( config["APPLICATION"]["controller_enabled"] );
    telnet_enabled = bool( config["APPLICATION"]["telnet_enabled"] );

    if "http_enabled" in config["APPLICATION"]:
        http_enabled = bool( config["APPLICATION"]["http_enabled"]);
    else:
        http_enabled = False

    if telnet_enabled:
        telnet_port = int( config["APPLICATION"]["telnet_port"] )
        telnet_host = config["APPLICATION"]["telnet_host"]

    beagle_environment.set_config("app_name", app_name)
    if not loading_external:
        beagle_environment.set_config("app_dir", "client/applications/" + app_name +"/")
    else:
        beagle_environment.set_config("app_dir", target_application_folder+"/")

    if resource_json:
            asset_manager.compile(resource_json)

    import client.beagle.beagle_modules as BeagleContainer
    sys.modules['Beagle'] = BeagleContainer

    import client.beagle.Newfoundland as Newfoundland
    sys.modules['Newfoundland'] = Newfoundland


    loaded_external = False
    if (app_dir is not None) and (app_module is not None):
        app = client.apps.get_app_from_path( app_dir, app_module )
        loaded_external = True
    else:
        app = client.apps.get_app(app_name)
    app.controller_enabled = controller_enabled
    app.configure( config );

    import client.beagle.Newfoundland as Newfoundland
    sys.modules['Newfoundland'] = Newfoundland

    if(telnet_enabled):
        console = telnet_console(app, telnet_host, telnet_port)
    try:
        app.init()
    except Exception as e:
        beagle_halt(e)
    set_status("initialized application:" + app_name)
    if(app.controller_enabled):
        log.write(log.INFO, "{0} requesting controller input".format(app_dir))
        gamepad.init()

    if http_enabled:
        portlist = config["APPLICATION"]["http_port"].split(",")
        http_key = config["APPLICATION"]["http_key"]
        while http_server is None:
            for port in portlist:
                try:
                    print("Trying HTTP Port: {0}".format(port))
                    http_server = bgl_http_server( app, "127.0.0.1",int(port), http_key)
                except:
                    http_server = None
            print("No available ports found... retrying in 1 second")
            sleep(1)
        print("HTTP Server bound on port {0}".format(http_server.port))




def finalize():
    global app
    app.finalize()
    gc.collect()

def configure( configuration ):
    pass

def immediate_cycle():
    global console
    global http_server

    if console is not None:
        console.tick()
    if http_server is not None:
        http_server.tick()

def tick():

    beagle_engine.profiler.total_frame_time = beagle_engine.timing.get_hf_timer() - beagle_engine.profiler.start_tick
    beagle_engine.profiler.start_tick = beagle_engine.timing.get_hf_timer()
    beagle_engine.profiler.total_spin_time = beagle_engine.profiler.start_tick - beagle_engine.profiler.end_render

    set_status( beagle_engine.profiler.status_string())

    if(app.controller_enabled):
        gamepad.tick()
    try:
        app.tick()
    except Exception as e:
        beagle_halt(e)
    gc.collect()
    beagle_engine.profiler.end_tick = beagle_engine.timing.get_hf_timer()
    beagle_engine.profiler.total_tick_time = beagle_engine.profiler.end_tick - beagle_engine.profiler.start_tick

    #frames.swap_render()


def render():
    #frames.render_display()
    #try:
    #    app.render()
    #except Exception as e:
    #    beagle_halt(e)
    beagle_engine.profiler.draw_calls = 0
    beagle_engine.profiler.start_render = beagle_engine.timing.get_hf_timer()
    try:
        app.render()
    except Exception as e:
        beagle_halt(e)
    if( gui_console.active):
        gui_console.render()
    beagle_engine.profiler.end_render = beagle_engine.timing.get_hf_timer()
    beagle_engine.profiler.total_render_time = beagle_engine.profiler.end_render - beagle_engine.profiler.start_render
    #with blend.blendstate(blend.blendmode.alpha_over):
    #    render_status()

def render_test():

    test_primitive = primitive.primitive( primitive.draw_mode.TRIS,
    [
        (1.0, 1.0),
        (-1.0, 1.0),
        (-1.0,-1.0)
    ],
    [
        (0.0,0.0),
        (1.0,0.0),
        (1.0,1.0)
    ])

    test_shader = shaders.shader( "test/vert", "test/pixel", "shaders/" )
    test_primitive.render_shaded( test_shader )
    return

def _get_mf_area():
    return mouse_focused_area

def calculate_mouse_position(area,x,y):
    xt = min(max( 0, x - area.r[0] ),area.r[2])
    yt = min(max( 0, y - area.r[1] ),area.r[3])
    return [xt,yt]

SIGNAL_HANDLED      = True
SIGNAL_DISCARDED    = False

def set_screensize(w,h):
    beagle_environment.set_config("screen_dims", [ w, h ] )

def dispatch_mouseup(button,x,y):
    global mouse_focused_area
    if mouse_focused_area is not None:
        m_pos = calculate_mouse_position(mouse_focused_area,x,y);
        mouse_focused_area.rcv_mouse_button(button,m_pos[0],m_pos[1], down = False)
        mouse_focused_area = None
        return SIGNAL_HANDLED
    return SIGNAL_DISCARDED

def dispatch_mousedown(button,x,y):
    global mouse_focused_area
    __clickpos[0] = x
    __clickpos[1] = y

    area = ui_area.find_ui_area(x,y)
    if area is not None:
        m_pos = calculate_mouse_position(area,x,y);
        area.rcv_mouse_button(button,m_pos[0],m_pos[1], down = True)
        mouse_focused_area = area
        return SIGNAL_HANDLED
    return SIGNAL_DISCARDED

def dispatch_mousemotion(x,y):
    global __mpos
    global mouse_focused_area
    __mpos = [x,y]
    ui_area.set_absolute_mpos([x,y])
    if mouse_focused_area is None:
        area = ui_area.find_ui_area(x,y)
        if area is not None:
            area.rcv_mousemotion(*(calculate_mouse_position(area,x,y)))
            return SIGNAL_HANDLED
    else:
        mouse_focused_area.rcv_mousemotion(*(calculate_mouse_position(mouse_focused_area,x,y)))
        return SIGNAL_HANDLED
    return SIGNAL_DISCARDED

def dispatch_key(key,down):
    keyboard.update_key(key,down)
    KEY_ESCAPE = 33
    caret_target = caret.get_caret()
    if down:
        if key == KEY_ESCAPE:
            caret.drop()
            return

    if caret_target is not None:
        caret_target.rcv_key(key,down)


def dispatch_root_keybindings(key,down):
    pass

def dispatch_text(text):
    if gui_console.active:
        gui_console.recv_text(text)

    caret_target = caret.get_caret()
    if caret_target is not None:
        caret_target.rcv_text(text)
    return

def map_keycode(code_definition):
    parsed = code_definition.split(":")
    keyboard.map_keycode_to_name(parsed[0],int(parsed[1]))
    return
