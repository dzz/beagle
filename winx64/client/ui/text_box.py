import sys
from client.ctt2.status             import set_status
from client.ctt2.status             import set_error
from client.ui.areas                import ui_area
from client.gfx.rect                import rect_solid
from client.gfx.rect                import rect_vgrad
from client.gfx.label               import label
from client.gfx.text                import render_text
from client.ui.default_renderer     import default_renderer
from client.ui.mod_caret_handler    import mod_caret_handler
from client.ui.mod_text_editor      import mod_text_editor

from client.ui.controlled_eval      import evaluator

import client.ui.style      as style
import host

class text_box(ui_area):
    def __init__(self, text="", editable = False, padding = 4, default_eval = None, use_python = False ):
        global _tmplabel
        ui_area.__init__(self)
        self.label_color = style.get("default_label_color");
        self.default_eval = default_eval
        self.text = text
        self.label = label(self.text)
        self.use_python = use_python
        self.client_color   = style.get("window_client_color")
        self.renderers = [ default_renderer() ]
        self.editable = editable
        self.active_color = style.get("active_textbox_color")
        self.padding = padding
        self.evaluated = None
        self.editing = False

        if self.editable:
            self.add_modifier(mod_caret_handler())
            self.add_modifier(mod_text_editor())
            self.original_text = self.text

    def begin_edit(self):
        self.original_text = self.text

    def revert_edit(self):
        self.set_text(self.original_text)

    def end_edit(self):
        self.editing = False
        if self.use_python:
            try:
                self.evaluated = evaluator(self.text)
                set_status("res: {}".format(self.evaluated))
            except:
                default_eval = None
                if(self.default_eval == None):
                    default_eval = self.text
                else:
                    default_eval = self.default_eval
                set_error("noeval:`{}`\nexcp:{}".format(str(default_eval), sys.exc_info()[0]))
                self.evaluated = default_eval
            self.set_text(str(self.evaluated))

    def set_text(self, text):
        self.text = text
        if self.text is not "":
            self.label.set_text(text)
        else:
            self.label.set_text(self.original_text)

    def get_text(self):
        return self.text

    def mutate_layout_height(self,height):
        return 24

        
    def render_client_area(self):
        pad_px = self.padding
        if(self.editable): 
            if(self.has_caret()):
                rect_solid  ( self.get_dims() , self.active_color )
                render_text( self.text, pad_px, pad_px, self.label_color )
            else:
                self.label.draw(pad_px,pad_px, self.label_color)
        else:
            rect_vgrad( self.get_dims(), self.client_color[0], self.client_color[1] ) 
            self.label.draw(pad_px,pad_px, self.label_color)
