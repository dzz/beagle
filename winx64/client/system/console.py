from io import StringIO
import sys
from client.beagle.assets import assets
from client.gfx.text import render_text
from client.gfx.texture import texture
from client.gfx.framebuffer import framebuffer
from client.gfx.context import gfx_context
from code import InteractiveInterpreter
import client.gfx.shaders as shaders
import getpass
import beagle_runtime

class console():
    shader = shaders.get("hwgfx/console","hwgfx/console")
    tex_shader = shaders.get("hwgfx/no_transform","hwgfx/texture")
    impulse = 0.0
    wrap_chars = int(beagle_runtime.get_screen_height()/8)
    history_len = int(beagle_runtime.get_screen_height()/8)
    foreground_rgb = [ 0.0,0.8,0.0]
    shadow_rgb = [0.0,0.2,0.0]
    background_rgba = [ 0.0,0.0,0.0,0.2]
    interpreter = None
    active = False
    toggle_key = 'f12'
    submit_key = 'return'
    backspace_key = 'backspace'
    history_back_key = 'up'
    history_fwd_key = 'down' 
    command = ''
    console_buffer = StringIO()
    lines = [ '' ]
    current_line = ''
    prompt = ''
    seen = False
    owner = {}
    command_delta = 0
    command_history = []
    texture = None

    def set_texture(name):
        if name not in texture.texture_lookup:
            print("no such texture")
        console.texture = texture.texture_lookup[name]

    def list_textures():
        for key in texture.texture_lookup:
            print(key)
 
    def error_message(message):
        console.lines.append(message.split("\n"))
        console.set_prompt()

    def toggle():
        console.active = not console.active
        if not console.seen:
            console.fb = framebuffer.from_screen()
            console.owner["cons"] = console
            console.interpreter = InteractiveInterpreter( console.owner )
            console.print_banner()
            console.seen = True
            console.set_prompt()

    def backspace():
        console.impulse = 0.7
        console.current_line = console.current_line[0:-1]
        console.command = console.command[0:-1]
        console.set_prompt()
        
    def set_prompt():
        console.prompt = "{0}@beagle>{1}".format(getpass.getuser(),console.current_line)
        console.render_console_texture()

    def recv_text(text):
        console.impulse = 0.5
        console.current_line = console.current_line+text
        console.command = console.command + text
        console.set_prompt()

    def reset():
        #console.lines.append(console.prompt)
        console.command = ''
        console.current_line = ''

    def print_banner():
        console.lines.extend(["{0}".format(console),"-------------------",""])

    def capture_stdout():
        sys.stdout = console.console_buffer
        sys.stderr = console.console_buffer

    def release_stdout():
        sys.stdout = sys.__stdout__
        sys.stderr = sys.__stderr__

    def submit():
        def line_wrap(lines):
            out = []
            for line in lines:
                if len(line) < console.wrap_chars:
                    out.append(line) 
                else:
                    tmp = line
                    while(len(tmp)>0):
                        out.append(tmp[0:console.wrap_chars])
                        tmp = tmp[console.wrap_chars:]
            return out

        console.lines.extend(line_wrap([ console.current_line]))
        console.current_line = ''
        try:
            console.capture_stdout()
            result = console.interpreter.runsource(console.command+"\n")
            if(result is None):
                console.command = console.command+"\n"
            if(result is False):
                console.impulse = 0.3
                console.reset()
                console.set_prompt()
        except Exception as e:
            print("{0}".format(e))
            console.reset()
        finally:
            console.release_stdout()
            console.lines.extend(line_wrap(console.console_buffer.getvalue().split('\n')))
            console.console_buffer = StringIO()
            console.render_console_texture()

    def render_console_texture():
        renderable_lines = console.lines[-1*(console.history_len-2):]
        renderable_lines.append(console.prompt)

        with console.fb.as_render_target():
            gfx_context.clear_rgba( *console.background_rgba )
            if(console.texture):
                console.texture.render_processed(console.tex_shader)
            blendmode = assets.get("core/hwgfx/blendmode/alpha_over")
            with blendmode:
                for idx, line in enumerate(renderable_lines):
                    render_text( line, 0, -1+idx*8, console.shadow_rgb )
                    render_text( line, 0, idx*8, console.foreground_rgb )

    def render():
        console.impulse = console.impulse * 0.9
        blendmode = assets.get("core/hwgfx/blendmode/alpha_over")
        with blendmode:
            console.fb.render_shaded( console.shader, { "impulse" : console.impulse } );
        

