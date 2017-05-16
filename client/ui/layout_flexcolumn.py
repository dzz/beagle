class layout_flexcolumn:
    def __init__(self, weights=[1,1], row_height = None):
        total = float(sum(weights))

        self.normalized_weights = []
        for x in weights:
            self.normalized_weights.append( float(x) / total )

        self.num_columns = len(weights)
        self.row_height = row_height

    def perform_layout(self, ui_area):
        used_width = 0
        row_height = self.row_height

        if(row_height is None):
            row_height = ui_area.get_height() // (len(ui_area.children) // self.num_columns)

        for i in range(0, len(ui_area.children)):
            x = i % self.num_columns
            y = i // self.num_columns

            if x == 0:
                used_width = 0

            width = int(self.normalized_weights[x] * ui_area.get_width())

            child = ui_area.children[i]
            computed_row_height = child.mutate_layout_height(row_height)
            child.set_x(used_width)
            child.set_y( y * computed_row_height)
            child.set_width(width)
            child.set_height( computed_row_height )

            used_width += width

