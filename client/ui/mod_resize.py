from client.ui.mod_empty    import mod_empty
from client.ui.areas        import SIGNAL_EXIT_HANDLER
from client.ui.areas        import SIGNAL_CONTINUE_HANDLING
from client.ui.areas        import abs_mpos

class mod_resize(mod_empty):
    def __init__(self, handle_size):
        self.toggled = False
        self.resize_origin = [0,0]
        self.handle_size = handle_size

    def transform_client_area(self,r):
        return r

    def rcv_mouse_button(self,ui_area,button,x,y,down):
        [ gx, gy ] = abs_mpos()
        if down == False:
            self.toggled = False
            return False
        else:
            self.resize_origin = [gx, gy]
            if( (x > ( ui_area.r[2] - self.handle_size )) and
                (y > ( ui_area.r[3] - self.handle_size )) ):
                self.toggled = True
                return SIGNAL_EXIT_HANDLER

            return SIGNAL_CONTINUE_HANDLING

    def rcv_mousemotion(self,ui_area,x,y):
        [ x, y ] = abs_mpos()
        if self.toggled:
            ui_area.r[2] += x - self.resize_origin[0]
            ui_area.r[3] += y - self.resize_origin[1]
            ui_area.r[2] = max(ui_area.r[2],self.handle_size)
            ui_area.r[3] = max(ui_area.r[3],self.handle_size)
            self.resize_origin = [x,y]
            return SIGNAL_EXIT_HANDLER

        return SIGNAL_CONTINUE_HANDLING
