import  client.ui.style             as style
from    client.ui.area_renderer     import area_renderer
from    client.gfx.label            import label 
from    client.gfx.rect             import rect_solid
from    client.gfx.rect             import rect_vgrad


class window_renderer(area_renderer):
    def __init__(self,window):
        self.label = label( text        = window.title, 
                            fontsize    = style.get("window_titlebar_fontsize"))

        self.title_col      = style.get("window_titlebar_text_color")
        self.title_bgcol    = style.get("window_titlebar_bg_color")
        self.bgcol          = style.get("window_bg_color")

    def render(self, window):
        #rect_solid( window.r, self.bgcol)
        rect_vgrad( [window.r[0], window.r[1], window.r[2], window.titlebar_height],
                         self.title_bgcol[0], self.title_bgcol[1]  ) 


        rect_solid  ( [window.r[0]+window.r[2] - 9,
                           window.r[1]+window.r[3] - 9,
                           9, 
                           9 ], 
                           self.bgcol );

        self.label.draw (window.r[0], window.r[1],self.title_col)

