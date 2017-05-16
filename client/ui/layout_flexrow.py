class layout_flexrow:
    def __init__(self, weights=[1,1], row_width = None):
        total = float(sum(weights))

        self.normalized_weights = []
        for x in weights:
            self.normalized_weights.append( float(x) / total )

        self.num_rows = len(weights)
        self.row_width = row_width
    
    def perform_layout(self, ui_area):
        used_height = 0
        row_width = self.row_width

        if(row_width is None):
            row_width = ui_area.get_width() // (len(ui_area.children) // self.num_rows)

        for i in range(0, len(ui_area.children)):
            x = i // self.num_rows
            y = i % self.num_rows

            if y == 0:
                used_height = 0

            height = int(self.normalized_weights[y] * ui_area.get_height())

            child = ui_area.children[i]
            child.set_x( x * row_width ) 
            child.set_y( used_height)
            child.set_width(child.mutate_layout_width( row_width))
            child.set_height(height)

            used_height += height

