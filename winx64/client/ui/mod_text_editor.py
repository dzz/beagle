from client.ui.mod_empty    import mod_empty

class mod_text_editor(mod_empty):
    def __init__(self):
        self.cursor = 0

    def rcv_key(self,ui_area,key,down):
        pass
        #if down:
        #    ui_area.set_text( ui_area.get_text() + chr(key) )

    def rcv_text(self,ui_area,text):
       ui_area.set_text( ui_area.get_text() + text )
