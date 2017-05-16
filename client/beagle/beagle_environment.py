
settings = {
        "render_status" : False
}


def set_config(key,value):
    global settings
    settings[key]=value

def get_config(key):
    global settings
    return settings[key]

def get(key):
    global settings
    return settings[key]
