import  client.ui.style             as style
from client.ui.areas                import ui_area
from client.ui.child_renderer       import child_renderer
from client.ui.mod_parent           import mod_parent
from client.gfx.label               import label 


class panel(ui_area):
    def __init__(self, children = None , layout = None ):
        ui_area.__init__(self)
        self.modifier_stack = [ mod_parent() ]
        self.renderers      = [ child_renderer(self)]

        if layout is not None:
            self.add_layout(layout)

        if children is not None:
            self.add_children(children)

