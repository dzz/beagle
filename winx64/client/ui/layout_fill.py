class layout_fill_window:
    def perform_layout(self,ui_area):
        r = ui_area.r
        ui_area.children[0].r = [ 0, 0, r[2], r[3] - ui_area.titlebar_height ]

class layout_fill:
    def perform_layout(self,ui_area):
        r = ui_area.r
        ui_area.children[0].r = [ 0, 0, r[2], r[3] ] 
