import platform

from client.beagle.beagle_api import api as BGL
from functools import partial

class Controllers():
    """ Class for proxying controller input to player objects

    Attributes:
        keyboard_debug (default False) - set to True to mock keyboard inputs to controller 0
    """
    keyboard_debug = False
    keyboard_left_stick = [ 0.0, 0.0 ]
    keyboard_right_stick = [ 0.0,0.0 ]


    def enable_keyboard_mode(self):
        def modKeyStick( stick, element, value):
            stick[element] = value
        Controllers.keyboard_debug = True
        BGL.keyboard.register_keyhandler_pair( "a",
            down = partial( modKeyStick, Controllers.keyboard_left_stick, 0, -1.0),
            up = partial( modKeyStick, Controllers.keyboard_left_stick, 0, 0.0)
        )
        BGL.keyboard.register_keyhandler_pair( "d",
            down = partial( modKeyStick, Controllers.keyboard_left_stick, 0, 1.0),
            up = partial( modKeyStick, Controllers.keyboard_left_stick, 0, 0.0)
        )
        BGL.keyboard.register_keyhandler_pair( "w",
            down = partial( modKeyStick, Controllers.keyboard_left_stick, 1, -1.0),
            up = partial( modKeyStick, Controllers.keyboard_left_stick, 1, 0.0)
        )
        BGL.keyboard.register_keyhandler_pair( "s",
            down = partial( modKeyStick, Controllers.keyboard_left_stick, 1, 1.0),
            up = partial( modKeyStick, Controllers.keyboard_left_stick, 1, 0.0)
        )
        BGL.keyboard.register_keyhandler_pair( "left",
            down = partial( modKeyStick, Controllers.keyboard_right_stick, 0, -1.0),
            up = partial( modKeyStick, Controllers.keyboard_right_stick, 0, 0.0)
        )
        BGL.keyboard.register_keyhandler_pair( "right",
            down = partial( modKeyStick, Controllers.keyboard_right_stick, 0, 1.0),
            up = partial( modKeyStick, Controllers.keyboard_right_stick, 0, 0.0)
        )
        BGL.keyboard.register_keyhandler_pair( "up",
            down = partial( modKeyStick, Controllers.keyboard_right_stick, 1, -1.0),
            up = partial( modKeyStick, Controllers.keyboard_right_stick, 1, 0.0)
        )
        BGL.keyboard.register_keyhandler_pair( "down",
            down = partial( modKeyStick, Controllers.keyboard_right_stick, 1, 1.0),
            up = partial( modKeyStick, Controllers.keyboard_right_stick, 1, 0.0)
        )

        BGL.keyboard.register_keyup_handler("tab", self.cycle_keyboard_player)


    def cycle_keyboard_player(self):
        self.keyboard_controls_player = (self.keyboard_controls_player + 1)%4;

    def __init__(self):

        self.keyboard_controls_player = 0
        if Controllers.keyboard_debug:
            Controllers.enable_keyboard_mode(self)

        # this axis mapping seems to work for xpad, not required
        # for xboxdrv/win32.
        #
        # BGL.gamepads.set_axis_order( [0,1,3,4,2,5] )

        self.synch()

    def synch(self):

        ## captures state of gamepads, dequeing events from the runtime.
        ## @TODO:dzz research hotplugging bugs

        self.virtualized_pads = [
            BGL.gamepads.by_index(0),
            BGL.gamepads.by_index(1),
            BGL.gamepads.by_index(2),
            BGL.gamepads.by_index(3)
        ]

    def tick(self):
        self.synch()

    def get_virtualized_pad(self, num):
        if Controllers.keyboard_debug:
            if num == self.keyboard_controls_player:
                pad = self.virtualized_pads[num]
                pad.left_stick[0] = Controllers.keyboard_left_stick[0]
                pad.left_stick[1] = Controllers.keyboard_left_stick[1]
                pad.right_stick[0] = Controllers.keyboard_right_stick[0]
                pad.right_stick[1] = Controllers.keyboard_right_stick[1]
                return pad
        return self.virtualized_pads[num]
