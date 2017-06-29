import hwgfx

class blendmode:
    alpha_over        = 0
    brush_composite   = 1
    dab_rendering     = 2
    blackhole         = 600
    add               = 5000
    darken            = 6000
    darken2           = 6001

class blendstate:
    stack = []
    def __init__(self,mode):
        self.mode = mode

    def __enter__(self):
        blendstate.stack.append(self.mode)
        hwgfx.manual_blend_enter(self.mode)

    def __exit__(self, exc_type, exc_value, traceback):
        blendstate.stack.pop()

        if(len(blendstate.stack)>0):
            hwgfx.manual_blend_enter(blendstate.stack[-1])
        else:
            hwgfx.manual_blend_exit()

#mode_over              = 0
#mode_brush_composite   = 1
#mode_dab_rendering     = 2
#mode_blackhole         = 600
#mode_add               = 5000
#mode_darken            = 6000
#mode_darken2           = 6001
#
#named_modes = { "alpha" : mode_over, "add" : mode_add, "darken" : mode_darken, "darken2" : mode_darken2 };
#                   
#current_state          = -1
#current_managed_state  = -1
#
#class state:
#    def __init__(self,mode):
#        self.mode = mode
#
#    def __enter__(self):
#        global current_state
#        global current_managed_state
#        current_managed_state = self.mode
#        current_state         = self.mode
#        hwgfx.blend_enter(self.mode)
#
#    def __exit__(self, exc_type, exc_value, traceback):
#        current_managed_state = -1
#        current_state = -1
#        hwgfx.blend_exit()
#
#def manual_blend_enter(mode):
#    global current_state
#    current_state = mode
#    hwgfx.manual_blend_enter(mode)
#
#def manual_blend_exit():
#    global current_state
#    global current_managed_state
#    current_state = current_managed_state
#    hwgfx.manual_blend_enter(current_state)
#
