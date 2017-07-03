import beagle_runtime

GamepadDeadzone = 0.15
GamepadFilter   = 0.5

gf_a = 1.0-GamepadFilter
gf_b = GamepadFilter


class pad_buttons:
    A = 0
    B = 1
    X = 2
    Y = 3
    BACK = 4
    GUIDE = 5
    START = 6
    LEFT_STICK = 7
    RIGHT_STICK = 8
    LEFT_BUMPER = 9
    RIGHT_BUMPER = 10
    DPAD_UP = 11
    DPAD_DOWN = 12
    DPAD_LEFT = 13
    DPAD_RIGHT = 14

class gamepad:
    axis_order = [ 0,1,3,4,2,5 ]
    def __init__(self):
        self.idx = 0
        self.leftStick = [0.0,0.0]
        self.rightStick = [0.0,0.0]
        #aliases
        self.left_stick = self.leftStick
        self.right_stick = self.rightStick
        self.triggers = [0.0,0.0]

    def button_down(self,btn):
        return beagle_runtime.get_gamepad_button( self.idx,btn) != 0

gamepads = [];

def get_gamepad(i = 0):
    if i < len(gamepads):
        return gamepads[i]
    return gamepad()

def init():
    max_pads = 8
    for i in range(0, max_pads ):
        gamepads.append( gamepad() );

def tick():
    if beagle_runtime.dequeue_gamepad_dirty() == 1:
        for i in range(0,beagle_runtime.get_gamepad_count() ):
            gp          = gamepads[i]

            axis_data   = beagle_runtime.get_gamepad_sticks(i)

            filtered_axis_data = [0.0,0.0,0.0,0.0,0.0,0.0]
            for ax in range(0,6):
                filtered_axis_data[ax] = axis_data[ax]
                if abs(axis_data[ax])<GamepadDeadzone:
                        filtered_axis_data[ax] = 0

            gp.idx = i
            gp.leftStick[0]  = gp.leftStick[0]*gf_a + filtered_axis_data[gamepad.axis_order[0]]*gf_b
            gp.leftStick[1]  = gp.leftStick[1]*gf_a + filtered_axis_data[gamepad.axis_order[1]]*gf_b
            gp.rightStick[0]  = gp.rightStick[0]*gf_a + filtered_axis_data[gamepad.axis_order[2]]*gf_b
            gp.rightStick[1]  = gp.rightStick[1]*gf_a + filtered_axis_data[gamepad.axis_order[3]]*gf_b
            gp.triggers[0] = filtered_axis_data[gamepad.axis_order[4]]
            gp.triggers[1] = filtered_axis_data[gamepad.axis_order[5]]

