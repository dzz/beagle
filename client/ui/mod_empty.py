from client.ui.areas import SIGNAL_EXIT_HANDLER
from client.ui.areas import SIGNAL_CONTINUE_HANDLING

class mod_empty(object):
    def __init__(self):
        pass

    def rcv_mousemotion(self,ui_area,x,y):
        return SIGNAL_CONTINUE_HANDLING

    def rcv_mouse_button(self,ui_area,button,x,y,down):
        return SIGNAL_CONTINUE_HANDLING

    def rcv_key(self,ui_area,key,down):
        return SIGNAL_CONTINUE_HANDLING

    def transform_client_area(self,r):
        return r

    def layout_children(self,children):
        pass

    def rcv_text(self,ui_area,text):
        pass
