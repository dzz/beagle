from io import StringIO
import sys
from client.beagle.assets import assets
from client.gfx.text import render_text
from code import InteractiveInterpreter

class console():
    interpreter = None
    active = False
    toggle_key = 'f12'
    submit_key = 'return'
    command = ''
    executor = None
    console_buffer = StringIO()
    lines = [ '' ]
    current_line = ''
    seen = False
    owner = {}

    def error_message(message):
        console.lines.append(message)

    def toggle():
        console.active = not console.active
        if not console.seen:
            console.interpreter = InteractiveInterpreter( console.owner )
            console.interpreter.write = console.error_message
            console.print_banner()

    def recv_text(text):
        console.current_line = console.current_line+text
        console.command = console.command + text
        console.lines[-1] = ">{0}]".format(console.current_line)

    def reset():
        console.command = ''

    def print_banner():
        console.lines.extend(["{0}".format(console),"-=~=-=~=-=~=-=~",""])

    def submit():
        console.current_line = ''
        sys.stdout = console.console_buffer
        try:
            result = console.interpreter.runsource(console.command)
            if(result is None):
                console.command = console.command+"\n"
            if(result is False):
                console.reset()
        except Exception as e:
            print("{0}".format(e))
            console.reset()
        finally:
            sys.stdout = sys.__stdout__
            console.lines.extend(console.console_buffer.getvalue().split('\n'))
            console.console_buffer = StringIO()
            console.lines.append("") 

    def render():
        blendmode = assets.get("core/hwgfx/blendmode/alpha_over")
        renderable_lines = console.lines[-30:]
        with blendmode:
            for idx, line in enumerate(renderable_lines):
                render_text( line, 0, idx*8, [0.0,0.7,0.0] ) 


