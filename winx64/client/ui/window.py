import host

from client.ui.areas            import ui_area

from client.ui.mod_resize       import mod_resize
from client.ui.mod_titlebar     import mod_titlebar
from client.ui.mod_parent       import mod_parent

from client.ui.window_renderer  import window_renderer
from client.ui.default_renderer import default_renderer
from client.ui.child_renderer   import child_renderer

from client.gfx.rect            import rect_solid
from client.gfx.rect            import rect_vgrad

import client.ui.style          as style



class window(ui_area):
    def __init__(self,title="ctt2_window",x=30,y=30,width=75,height=75,titlebar_height=12,handle_size = 9):
        ui_area.__init__(self)
        self.is_focusable = True
        self.set_display_area( [x,y,width,height] )

        self.title = title 
        self.titlebar_height = titlebar_height

        self.modifier_stack = [ mod_resize   ( handle_size = handle_size ),
                                mod_titlebar ( self, titlebar_text = self.title, height = self.titlebar_height ),
                                mod_parent   ( ) ]

        self.renderers = [
                            default_renderer(),
                            child_renderer(self),
                            window_renderer(self)
                         ]

        self.layouts = [ ]

        self.client_color      = style.get("window_client_color")

    def render_client_area(self):
        rect_vgrad  ( [0,0,self.r[2],self.r[3]], self.client_color[0],self.client_color[1])

