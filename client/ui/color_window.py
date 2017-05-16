from client.ui.window               import window
from client.ui.layout_grid          import layout_grid
from client.ui.color_box            import color_box

class color_window(window):
    def __init__(self):
        window.__init__(self,"colors",x=20,y=20,width=600,height=600)

        for i in range(0,256):
            x = float(i) / 255.0
            y = float(i % 16)/16.0
            self.add_child( color_box( color = [y*x,1-x,x*0.5-y,1] ) )

        self.add_layout( layout_grid(16,16) )
