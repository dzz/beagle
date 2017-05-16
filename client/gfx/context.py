import hwgfx

def set_clear_color(r,g,b,a):
    hwgfx.set_clear_color(r,g,b,a)

def clear():
    hwgfx.clear()

class gfx_context():
    def clear(color):
        set_clear_color(color[0],color[1],color[2],color[3])
        clear()

    def clear_rgba(r,g,b,a):
        set_clear_color(r,g,b,a)
        clear()
