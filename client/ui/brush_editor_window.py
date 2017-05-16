from enum import Enum

from client.ui.areas                import ui_area
from client.ui.property_editor      import property_editor
from client.ui.window               import window
from client.ui.layout_flexcolumn    import layout_flexcolumn
from client.ui.layout_flexrow       import layout_flexrow
from client.ui.mod_parent           import mod_parent
from client.ui.child_renderer       import child_renderer
from client.ui.default_renderer     import default_renderer
from client.gfx.rect                import rect_vgrad
from client.gfx.text                import render_text
from client.ui.panel                import panel
from client.ui.layout_fill          import layout_fill
from client.ui.layout_fill          import layout_fill_window

import client.ui.style      as style


class node_connection:
    node    = None
    channel = None

class contype(Enum):
    float = 0
    vec3  = 1

class plug:
    node = None

class jack:
    node = None
    name = ""
    contype = contype.float
    plugs = []
    value = 0.0

    def set_value(self, v):
        self.value = v

class node:
    name            = "None"
    inputs          = []
    outputs         = []
    #deprecated
    input_names     =[]
    output_names    =[]
    #end deprecate
    property_map    = {}


class node_passthru(node):
    name = "passthru"
    input_names     = ["in" ]
    output_names    = ["out" ]

class node_function(node):
    name = "function"
    input_names     = ["x" ]
    output_names    = ["y" ]
    property_map    = {"funcdef":"\"y=sin(x)\""}

class node_system(node):
    name = "system"
    output_names    = ["time(ms)"]

class node_document(node):
    name = "workspace"
    output_names    = ["frame #"]

class node_stylus(node):
    name = "stylus"
    output_names    = ["position","pressure","azimuth","rotation"]
    property_map    = { "rot smooth"    : "0.6", 
                        "pos smooth"    : "0.8" }

class node_shader_output(node):
    name = "shader"
    input_names     = [ "position","radius","base_color","noise","rotation" ,"alpha"]

class connection_area(ui_area):

    padding = 2

    def __init__(self, name, is_input = True):
        self.label_color        = style.get("default_label_color")
        self.input_tint_color   = style.get("input_tint_color")
        self.output_tint_color   = style.get("output_tint_color")

        self.is_input = is_input
        ui_area.__init__(self)
        self.name = name 
        self.renderers = [ default_renderer() ]

    def render_client_area(self):
        if(self.is_input):
            rect_vgrad  ( [0,0,self.r[2],self.r[3]], self.input_tint_color , [1.0,1.0,1.0,0.0] )
        else:
            rect_vgrad  ( [0,0,self.r[2],self.r[3]], self.output_tint_color, [1.0,1.0,1.0,0.0] )

        render_text( self.name, self.padding, self.padding, self.label_color )


class node_window(window):
    node = node()

    def __init__(self, node = None):

        if node is not None:
            self.node = node

        window.__init__(self, title=self.node.name )


        conareas_in = []
        conareas_out = []

        for n in self.node.input_names:
            conarea = connection_area( n, is_input = True )
            conareas_in.append(conarea)

        for n in self.node.output_names:
            conarea = connection_area( n, is_input = False )
            conareas_out.append(conarea)

        cons_in_panel   = panel( children = conareas_in,  layout = layout_flexcolumn( weights = [1] ))
        cons_out_panel  = panel( children = conareas_out, layout = layout_flexcolumn( weights = [1] ))

        spacer = panel()

        
        if( conareas_in != [] and conareas_out !=[]):
            cons_panel = panel( 
                                children    = [ cons_in_panel, spacer, cons_out_panel], 
                                layout      = layout_flexcolumn( weights = [1,0.05,1] ) 
                                )
        elif (conareas_in!=[]):
            cons_panel = panel( 
                                children    = [ cons_in_panel, spacer ],
                                layout      = layout_flexcolumn( weights = [1,0.05] ) 
                                )
        elif (conareas_out!=[]):
            cons_panel = panel( 
                                children    = [ spacer, cons_out_panel ],
                                layout      = layout_flexcolumn( weights = [0.05,1.0] ) 
                                )

        editor_panel = panel( 
                                children  = [ property_editor( self.node.property_map ) ], 
                                layout    = layout_fill() 
                            )


        self.add_children( [ cons_panel, editor_panel ] )
        self.add_layout( layout_flexrow( weights=[1,1] ) )
        

class node_editor(ui_area):
    def __init__(self):
        ui_area.__init__(self)
        self.renderers = [ default_renderer(), child_renderer(self) ]
        self.add_modifier( mod_parent() )
        self.add_children(
                [   node_window( node = node_shader_output()    ),
                    node_window( node = node_passthru()         ),
                    node_window( node = node_system()           ),
                    node_window( node = node_document()         ),
                    node_window( node = node_stylus()           ),
                    node_window( node = node_function()         ) ])


    def render_client_area(self):
        rect_vgrad  ( [0,0,self.r[2],self.r[3]], [1.0,1.0,1.0,1.0],[0.5,0.5,0.6,1.0] )

class brush_editor_window(window):
    def __init__(self):

        property_map = {"color" :"[0.0,0.0,0.0]", "test":"hello world"}

        window.__init__(self,title="brush editor" ,x=0,y=0,width=600,height=400)

        self.add_child( node_editor() )
        self.add_layout( layout_fill_window() )

