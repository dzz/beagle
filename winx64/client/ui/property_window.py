from client.ui.property_editor      import property_editor
from client.ui.window               import window
from client.ui.layout_fill          import layout_fill_window

class property_window(window):
    def __init__(self, title = "properties", property_map = {"default": ""} ):
        window.__init__(self,title ,x=0,y=0,width=400,height=400)
        self.add_child( property_editor( property_map ) )
        self.add_layout( layout_fill_window() )

