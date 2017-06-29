import hwgfx

def render_text(text, x, y, color=[1.0,1.0,1.0]):
    hwgfx.text_render(x,y,color[0],color[1],color[2],text)
