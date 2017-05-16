from client.beagle.assets import assets
from client.gfx.primitive import primitive
from client.gfx.text import render_text
from client.gfx.framebuffer import render_target
from client.gfx.context import gfx_context
from client.system.gamepad import pad_buttons, gamepad
from client.beagle.assets import assets

## Ultimately all official API features need to have entry points from here, presently migrating
## on as-needed basis from old mechanisms which either necessitated tons of import statements, or
## using the hash in the asset manager




class beagle_api():
    """ High Level Aggregate API for Beagle 
    
        Attributes: 
            assets: a handle to the beagle asset manager
    """
    assets = assets

    class curve_driver():
        """ tickable binding for a curve 
        """
        def __init__(self,**kwargs):
            self.curve = kwargs['curve']
            self.rate = kwargs['rate']
            self.t = 0

        def tick(self):
            self.t += self.rate
        
        def value(self):
            return self.curve.value_at(self.t)

        def is_finished(self):
            if( self.t > self.curve.max_t ):
                return True

    class simple_tick_manager():
        """ Simple Tick Manager Superclass

                 Inherit from this and add tickables with create_tickable, tick will auto tick linked tickables. 
        """
        def __init__(self):
            self.tickables = []
    
        def create_tickable(self, tickable):
            """ append a tickable to the list of objects to tick. returns the bound tickable object. """
            self.tickables.append( tickable )
            return tickable
    
        def tick(self):
            """ tick all tickables """
            for tickable in self.tickables:
                tickable.tick()

    class purging_tick_manager( simple_tick_manager):
        """ Purging Tick Manager Superclass

                tick manager which cleans up tickables where tick does not return True
        """
        def tick(self):
            """ tick all tickables, removing ones for which tick does not return True """
            self.tickables = list(filter( lambda tickable: tickable.tick(), self.tickables ))

    class gamepads():
        """ Gamepad API
        
            Attributes:
                buttons: a map of identifiers (e.g. A,B,X,Y) to button indices
        """
        buttons = pad_buttons

        def set_axis_order( axis_order ):
            gamepad.axis_order = axis_order

        def by_index(index):
            """ returns a gamepad object by player index """
            return assets.exec("core/queries/gamepad/find_by_id[id]", [index] )
        def find_primary():
            """ returns the primary gamepad """
            return assets.exec("core/queries/gamepad/find_primary")


    class primitive():
        """ Primitive API
        
            Attributes:
                unit_uv_square: a unit square (-1.0,1.0) primitive useful as input data for a shader
        """
        unit_uv_square = primitive.get_unit_uv_primitive()

    class context():
        """ Graphics Context API """
        def clear(r,g,b,a):
            """ clear context to specific color """
            return gfx_context.clear_rgba(r,g,b,a)

        def render_target(buffer):
            """ returns a context manger, render calls in context will apply to the passed in framebuffer """
            return render_target(buffer)


    class framebuffer():
        """ Framebufer API """
        def from_dims(w,h, **kwargs):
            """  Create a frame buffer with specified dimensions """
            if "filtered" in kwargs:
                return assets.exec("core/factory/framebuffer/[w,h]",[w,h, kwargs['filtered']])
            else:
                return assets.exec("core/factory/framebuffer/[w,h]",[w,h])

        def from_screen():
            """  Create a frame buffer with the same dimensions as the primary render target"""
            return assets.exec("core/factory/framebuffer/from_screen")

    class view():
        widescreen_16_9 = assets.get("beagle-2d/coordsys/16:9")

    class blendmode():
        """ Blendmode API 

            Attributes:
                alpha_over: blendstate for standard alpha channel blending
                add: blendstate for additions
        """
        alpha_over = assets.get("core/hwgfx/blendmode/alpha_over")
        add = assets.get("core/hwgfx/blendmode/add")

    class lotext():
        """ Lotext API, low level text rendering. """
        def render_text_pixels( txt, x, y, color ):
            """ Render text at an arbitrary pixel position with color in form [r,g,b] """
            return render_text(txt,x,y, color)

        def render_text_grid( txt, x, y, color):
            """ Render text at an arbitrary (8x8) character grid position with color in form [r,g,b] """
            return render_text(txt,x*8,y*8, color)
    
    class game():
        def __init__(self):
            pass
        def init(self):
            pass
        def render(self):
            pass
        def tick(self):
            pass
        def finalize(self):
            pass
        def configure(self, application_ini):
            pass

api = beagle_api

class texture():
    def get_label( txt, **kwargs):
        fb = beagle_api.framebuffer.from_dims( kwargs['dims'][0], kwargs['dims'][1], filtered = False )
        with beagle_api.context.render_target(fb):
            beagle_api.lotext.render_text_pixels(txt, 0,0, kwargs['rgb'] )
        return fb2.get_texture()

class basic_sprite_renderer():
    shader = beagle_api.assets.get("beagle-2d/shader/beagle-2d")
    def render(self):
        beagle_api.primitive.unit_uv_square.render_shaded( basic_sprite_renderer.shader, self.get_shader_params() )


api.texture = texture
api.basic_sprite_renderer = basic_sprite_renderer

