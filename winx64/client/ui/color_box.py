from client.ui.areas            import ui_area
from client.gfx.rect            import rect_solid
from client.ui.default_renderer import default_renderer

class color_box(ui_area):
    def __init__(self, color = [0.0,0.0,0.0,0.0] ):
        ui_area.__init__(self)
        self.renderers = [ default_renderer() ]
        self.color = color

    def render_client_area(self):
        rect_solid( [0,0,self.r[2],self.r[3]], [0.0,0.0,0.0,1.0])
        rect_solid( [1,1,self.r[2]-1,self.r[3]-1], self.color)

