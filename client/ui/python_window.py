import host
from client.ui.property_window      import property_window
import client.ui.controlled_eval


class python_window(property_window):
    def __init__(self):
        property_window.__init__(self, title="python", property_map = {"input":""} )

