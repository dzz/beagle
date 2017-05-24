scan_dict   = {}
keys        = {} 

key_down_handlers   = {}
key_up_handlers     = {}
key_press_handlers  = {}

def map_keycode_to_name(name, keycode):
    global scan_dict
    global keys
    global key_handlers
    global key_down_handlers
    global key_up_handlers
    global key_press_handlers

    keys[ name ]                   = False
    key_down_handlers[ name ]      = []
    key_up_handlers[ name ]        = []
    key_press_handlers[ name ]     = []

    scan_dict[ keycode ]           = name


def update_key(keycode, down):
    global scan_dict
    global keys
    global key_handlers
    global key_down_handlers
    global key_up_handlers
    global key_press_handlers

    key_name = scan_dict[keycode]
    pressed = False

    if not down and keys[ key_name ]:
        pressed = True

    keys[ key_name ] = down

    if pressed:
        for handler in key_press_handlers[ key_name ]:
            handler()

    if down:
        for handler in key_down_handlers[ key_name ]:
            handler()
    else:
        for handler in key_up_handlers[key_name]:
            handler()

def register_keydown_handler( key_name, handler ):
    global key_down_handlers
    key_down_handlers[key_name].append(handler)

def register_keyup_handler( key_name, handler ):
    global key_up_handlers
    key_up_handlers[key_name].append(handler)

def register_keypress_handler( key_name, handler ):
    global key_press_handlers
    key_press_handlers[key_name].append(handler)

def register_keyhandler_pair( key_name, **kwargs ):
    register_keydown_handler( key_name, kwargs['down'] )
    register_keyup_handler( key_name, kwargs['up'] )

