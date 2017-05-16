class layout_grid:
    def __init__(self, x=4,y=4):
        self.x = x
        self.y = y

    def set_dims(x,y):
        self.x = x
        self.y = y

    def perform_layout(self, ui_area):
        width   = ui_area.r[2] // self.x
        height  = ui_area.r[3] // self.y

        for i in range(0, len(ui_area.children)):
            x                   = (i % self.x) * width
            y                   = (i // self.y) * height
            child               = ui_area.children[i]
            mutated_height      = child.mutate_layout_height(height)
            child.r             = [x,y,width,mutated_height]
