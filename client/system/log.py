import beagle_runtime

ERROR = 1
WARNING = 2
INFO = 4
DEBUG = 8
GFX_MSG = 16
AUD_MSG = 32

def set_level(level):
    beagle_runtime.log_set_level(level)

def write(level,txt):
    beagle_runtime.log_client_message(level,txt)
