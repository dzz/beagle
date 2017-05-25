from io import StringIO
import sys
from client.beagle.assets import assets
from client.gfx.text import render_text

class console():
    active = False
    toggle_key = 'f12'
    submit_key = 'return'
    command = ''
    executor = None
    console_buffer = StringIO()
    lines = [ '' ]
    seen = False

    def toggle():
        console.active = not console.active
        if not console.seen:
            console.print_banner()

    def recv_text(text):
        console.command = console.command + text
        console.lines[-1] = "[{0}]".format(console.command)

    def reset():
        console.command = ''

    def print_banner():
        console.lines.extend(["{0}".format(console),"-=~=-=~=-=~=-=~",""])

    def submit():
        sys.stdout = console.console_buffer
        try:
            compiled = compile( console.command, 'console','eval' )
            if console.executor:
                print(console.executor.exec_console_code(compiled))
        except Exception as e:
            print("{0}".format(e))
        finally:
            sys.stdout = sys.__stdout__
            console.lines.extend(console.console_buffer.getvalue().split('\n'))
            console.console_buffer = StringIO()
            console.lines.append("") 

        console.reset()

    def render():
        blendmode = assets.get("core/hwgfx/blendmode/alpha_over")
        renderable_lines = console.lines[-30:]
        with blendmode:
            for idx, line in enumerate(renderable_lines):
                render_text( line, 0, idx*8, [0.0,1.0,0.0] ) 


