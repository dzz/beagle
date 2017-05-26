from io import StringIO
import sys
from client.beagle.assets import assets
from client.gfx.text import render_text
from code import InteractiveInterpreter
import getpass

class console():
    interpreter = None
    active = False
    toggle_key = 'f12'
    submit_key = 'return'
    backspace_key = 'backspace'
    command = ''
    console_buffer = StringIO()
    lines = [ '' ]
    current_line = ''
    prompt = ''
    seen = False
    owner = {}

    def error_message(message):
        console.lines.append(message.split("\n"))
        console.set_prompt()

    def toggle():
        console.active = not console.active
        if not console.seen:
            console.interpreter = InteractiveInterpreter( console.owner )
            #console.interpreter.write = console.error_message
            console.print_banner()
            console.seen = True
            console.set_prompt()

    def backspace():
        console.current_line = console.current_line[0:-1]
        console.command = console.command[0:-1]
        console.set_prompt()
        
    def set_prompt():
        console.prompt = "{0}@beagle>{1}".format(getpass.getuser(),console.current_line)

    def recv_text(text):
        console.current_line = console.current_line+text
        console.command = console.command + text
        console.set_prompt()

    def reset():
        console.lines.append(console.prompt)
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
        console.current_line = ''
        try:
            console.capture_stdout()
            result = console.interpreter.runsource(console.command)
            if(result is None):
                console.command = console.command+"\n"
            if(result is False):
                console.reset()
                console.set_prompt()
        except Exception as e:
            print("{0}".format(e))
            console.reset()
        finally:
            console.release_stdout()
            console.lines.extend(console.console_buffer.getvalue().split('\n'))
            console.console_buffer = StringIO()

    def render():
        blendmode = assets.get("core/hwgfx/blendmode/alpha_over")
        renderable_lines = console.lines[-60:]
        renderable_lines.append(console.prompt)
        with blendmode:
            for idx, line in enumerate(renderable_lines):
                render_text( line, 0, idx*8, [0.0,0.7,0.0] ) 


