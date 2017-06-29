from client.ui.areas                import ui_area
from client.ui.mod_parent           import mod_parent
from client.ui.layout_flexcolumn    import layout_flexcolumn
from client.ui.text_box             import text_box
from client.ui.default_renderer     import default_renderer
from client.ui.child_renderer       import child_renderer

class layout_rowborders:
    def __init__(self, row_border = 5):
        self.row_border = row_border

    def perform_layout(self,ui_area):
        for child in ui_area.children:
            child.set_height( child.get_height() - self.row_border );

class property_editor(ui_area):
    def __init__(self, property_map = {"default": ""} ):
        ui_area.__init__(self)

        self.add_modifier( mod_parent() )

        for k in property_map.keys():
            self.add_child( text_box( text=k ) )
            self.add_child( text_box( text=property_map[k], editable=True, use_python=True ) )

        self.renderers = [ child_renderer(self) ]

        self.add_layout( layout_flexcolumn(weights = [2,3], row_height = None) )
        self.add_layout( layout_rowborders() )

