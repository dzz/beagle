import hwgfx

class viewport: 
    @staticmethod
    def set_dimensions(x,y,w,h):
        hwgfx.viewport_set(x,y,w,h)

    @staticmethod
    def reset():
        hwgfx.viewport_reset()

class nested_viewport:
    def __init__(self):
        self.ports = []

    def enter_subview(self,x,y,w,h):
        if(len(self.ports) is not 0):
            port = self.ports[ len(self.ports) -1 ]
            xt = port[0] + x
            yt = port[1] + y

            new_right = xt+w
            new_bottom = yt+h
            current_right = port[0]+port[2]
            current_bottom = port[1]+port[3]

            if(new_right>current_right):
                w -= (new_right-current_right)

            if(new_bottom>current_bottom):
                h -= (new_bottom-current_bottom)

        else:
            xt = x
            yt = y


        self.ports.append([xt,yt,w,h])
        viewport.set_dimensions(xt,yt,w,h)

    def exit_subview(self):
       self.ports.pop()
       if(len(self.ports)==0):
           viewport.reset()
       else:
           port = self.ports[len(self.ports)-1]
           viewport.set_dimensions(*port)
        
_nv = nested_viewport()

class nested_view:
    def __init__(self,x,y,w,h):
        global _nv
        self._nv = _nv
        self.dims = [x,y,w,h]

    def __enter__(self):
        self._nv.enter_subview(*self.dims)

    def __exit__(self,exc_type,exc_value,traceback):
        self._nv.exit_subview()

def get_viewport_context():
    global _nv
    return _nv
