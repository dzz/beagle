import client.beagle.caret as caret

SIGNAL_EXIT_HANDLER         = True
SIGNAL_CONTINUE_HANDLING    = False

class ui_area(object):
    def __init__(self):
        self.r = [0,0,0,0]
        self.client_area = [0,0,0,0]
        self.z = 0;
        self.m_pos = [0,0]
        self.client_m_pos = [0,0]
        self.modifier_stack = []
        self.layouts = []
        self.renderers = []
        self.children = []
        self.parent= None
        self.is_focusable = False  #if true, z order updates on click

    def has_caret(self):
        if self == caret.get_caret():
            return True

    def add_layout(self,layout):
        self.layouts.append(layout)

    def add_modifier(self,modifier):
        self.modifier_stack.append(modifier)

    def layout(self):
        if( self.children != [] ):
            for layout in self.layouts:
                layout.perform_layout(self)
            if len(self.layouts) > 0:
                for modifier in self.modifier_stack:
                    modifier.layout_children(self.children)

    def set_display_area(self,r):
        self.r = r

    def get_display_area(self):
        return self.r

    def get_width(self):
        return self.r[2]

    def get_height(self):
        return self.r[3]

    def set_width(self,w):
        self.r[2] = w

    def set_height(self,h):
        self.r[3] = h

    def set_x(self,x):
        self.r[0] = x

    def set_y(self,y):
        self.r[1] = y

    def get_x(self):
        return self.x

    def get_y(self):
        return self.y

    def get_children(self):
        return self.children

    def get_dims(self):
        return [0,0,self.r[2],self.r[3]]

    def add_child(self,ui_area):
        self.children.append(ui_area)
        ui_area.parent = self

    def add_children(self,areas):
        for ui_area in areas:
            self.add_child(ui_area)

    def compute_client_area(self):
        self.client_area = list(self.r)

    def get_client_area(self):
        return self.client_area

    def rcv_mouse_button(self,button,x,y,down):
        if self.is_focusable:
            self.bring_top()
        for modifier in self.modifier_stack:
            if modifier.rcv_mouse_button(self,button,x,y,down) == SIGNAL_EXIT_HANDLER:
                return SIGNAL_EXIT_HANDLER;
        return SIGNAL_CONTINUE_HANDLING

    def rcv_mousemotion(self,x,y):
        self.set_m([x,y])
        for modifier in self.modifier_stack:
            if modifier.rcv_mousemotion(self,x,y) == SIGNAL_EXIT_HANDLER:
                return SIGNAL_EXIT_HANDLER;
        return SIGNAL_CONTINUE_HANDLING

    def rcv_key(self,key,down):
        for modifier in self.modifier_stack:
            if modifier.rcv_key(self,key,down) == SIGNAL_EXIT_HANDLER:
                return SIGNAL_EXIT_HANDLER;
        return SIGNAL_CONTINUE_HANDLING

    def rcv_text(self,text):

        #this is coupled to the caret handling
        #so we don't care about SIGNAL_EXIT_HANDLER etc. here,
        #the caret is by nature a singleton

        for modifier in self.modifier_stack:
            modifier.rcv_text(self,text)

    def set_m(self,position):
        self.m_pos = position
        self.client_m_pos[0] = self.m_pos[0]
        self.client_m_pos[1] = self.m_pos[1]

    def get_siblings(self):
        if self.parent is None:
            return get_ui_areas()
        else:
            return self.parent.children

    def mutate_layout_height(self,height):
        return height

    def mutate_layout_width(self,width):
        return width

    def order_siblings(self):
        _zsort(self.get_siblings())

    def bring_top(self):
        for area in self.get_siblings():
            area.z +=1
        self.z = 0
        self.order_siblings()
        order_areas()

    def render_client_area(self):
        pass

#controller

areas   = []
__mpos  = [0,0]

def register_ui_area(area):
    get_ui_areas().append(area)

def remove_ui_area(area):
    get_ui_areas().remove(area)

def order_areas():
    return _zsort(get_ui_areas())

def get_ui_areas():
    global areas
    return areas

def find_ui_area(x,y):
    rev = list(order_areas())
    rev.reverse()
    for area in rev:
        if( x >= area.r[0] and x < area.r[0] + area.r[2] and
                y >= area.r[1] and y < area.r[1] + area.r[3] ):
            return area
    return None

def set_absolute_mpos(mpos):
    global __mpos
    __mpos = mpos

def abs_mpos():
    global __mpos
    return __mpos

def _zsort(has_z):
    has_z.sort( key=lambda x: x.z, reverse = True )
    return has_z


def xy_in_r(x,y,r):
    return (x>=r[0] 
            and y>=r[1] 
            and (x < r[0]+r[2]) 
            and (y < r[1]+r[3]))
