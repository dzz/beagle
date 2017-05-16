import host

from client.ctt2.mouse_focus import capture_mouse
from client.ui.mod_empty    import mod_empty
from client.ui.areas        import SIGNAL_EXIT_HANDLER
from client.ui.areas        import SIGNAL_CONTINUE_HANDLING
from client.ui.areas        import xy_in_r

def get_reversed(l):
    rv = list(l)
    rv.reverse()
    return rv

def find_child(x,y,children):
    for child in get_reversed(children):
        if(xy_in_r(x,y,child.r)):
            xt = x - child.r[0];
            yt = y - child.r[1];
            return [child,xt,yt]
    return None

class mod_parent(mod_empty):
    def __init__(self):
        self.focused_area = None

    def rcv_mousemotion(self,ui_area,x,y):
        if(self.focused_area is not None):
            xt = x - self.focused_area.r[0];
            yt = y - self.focused_area.r[1];
            return self.focused_area.rcv_mousemotion(xt,yt)

        child_data = find_child(x,y,ui_area.children)
        if(child_data is not None):
            [child,xt,yt] = child_data;
            return child.rcv_mousemotion(xt,yt)
        return SIGNAL_CONTINUE_HANDLING

    def rcv_mouse_button(self,ui_area,button,x,y,down):
        if( down is False and self.focused_area is not None):
            xt = x - self.focused_area.r[0];
            yt = y - self.focused_area.r[1];
            rval = self.focused_area.rcv_mouse_button(button,xt,yt,down)
            self.focused_area = None
            return rval
        else:
            child_data = find_child(x,y,ui_area.children)
            if(child_data is not None):
                [child,xt,yt] = child_data;
                self.focused_area = child
                return child.rcv_mouse_button(button,xt,yt,down)
