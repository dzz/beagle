from client.gfx.text    import *

_status_str = ""
status_col = None

status_col_go   = [0.0,1.0,0.0]
status_col_stop = [1.0,0.0,0.0]

def _set_text(text):
    global _status_str
    _status_str = text

def set_status(text):
    global status_col
    status_col = status_col_go
    _set_text(text)

def set_error(text):
    global status_col
    status_col = status_col_stop
    _set_text(text)

def render_status():
    global _status_str
    global status_col
    render_text( _status_str, 0,0, status_col )
