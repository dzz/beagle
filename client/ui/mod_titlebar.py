from client.ui.mod_empty    import mod_empty
from client.ui.areas        import SIGNAL_EXIT_HANDLER
from client.ui.areas        import SIGNAL_CONTINUE_HANDLING
from client.ui.areas        import abs_mpos

class mod_titlebar(mod_empty):
    def __init__(self,ui_area,titlebar_text,height):
        self.cursor_origin = [0,0]
        self.origin = [0,0]
        self.height = height
        self.toggled = False

    def layout_children(self,children):
        for child in children:
            child.r[1] += self.height

    def rcv_mouse_button(self,ui_area,button,x,y,down):
        if down == False:
            self.toggled = False
            return SIGNAL_CONTINUE_HANDLING 
        else:
            if( y < self.height ):
                self.toggled = True
                self.origin = [ ui_area.r[0], ui_area.r[1] ]
                mpos = abs_mpos()
                self.cursor_origin = [ mpos[0], mpos[1] ]
                return SIGNAL_EXIT_HANDLER

    def rcv_mousemotion(self,ui_area,x,y):
        if self.toggled == False:
            return SIGNAL_CONTINUE_HANDLING
        else:
            mpos = abs_mpos()
            ui_area.r[0] = self.origin[0] + mpos[0] - self.cursor_origin[0]
            ui_area.r[1] = self.origin[1] + mpos[1] - self.cursor_origin[1]
            self.move_origin = [x,y]

            ui_area.r[0] = max(0, ui_area.r[0])
            ui_area.r[1] = max(0, ui_area.r[1])
            return SIGNAL_EXIT_HANDLER
