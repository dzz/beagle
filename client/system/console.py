class console():
    active = False
    toggle_key = 'f12'
    submit_key = 'return'
    command = ''
    executor = None

    def toggle():
        console.active = not console.active

    def recv_text(text):
        command = command + text

    def reset():
        console.command = ''

    def submit():
        console.reset()


