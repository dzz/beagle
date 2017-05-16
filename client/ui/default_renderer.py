from    client.gfx.viewport         import nested_view
from    client.ui.area_renderer     import area_renderer

class default_renderer(area_renderer):
    def __init__(self):
        pass

    def render(self, ui_area):
        with nested_view(*(ui_area.r)):
            ui_area.render_client_area()

