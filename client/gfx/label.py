import hwgfx
import client.system.log as log

class label:
    def __init__(self, text, fontsize = 8 ):
        self._label = hwgfx.label_generate()
        hwgfx.label_set_text(self._label,text)
        log.write(log.DEBUG, "acquired label {0}".format(self._label))

    def __del__(self):
        hwgfx.label_drop(self._label)
        log.write(log.DEBUG, "dropped label {0}".format(self._label))

    def set_text(self, text):
        hwgfx.label_set_text(self._label,text)

    def draw(self,x,y, color):
        hwgfx.label_render(self._label,x,y,color[0],color[1],color[2]);
