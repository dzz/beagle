Y_Axis_Up       = 1.0
Y_Axis_Down     = -1.0

def centered_view( Width, Height, Y_Axis = Y_Axis_Down ):
    return [ 1.0/(Width/2), Y_Axis/(Height/2) ]

