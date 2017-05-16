# Creating an Application 

## Step 1 - Create the application directory

The application can be loaded from the internal client/applications/ directory, or from an external directory.

If you wish to load from an external directory, you must specify in application.ini:

```
; inside this path must be a package named 'app'
app_path=c:\my_code\my_game
;
; this will load c:\my_code\my_game\app\main.py
; and requires
; 
; c:\my_code\my_game\app\__init__.py   
app_module=main
```

## Step 2 - Create the Python module

You will need to create an *``__init__.py``* inside this directory so python recognizes it as a module. 


## Step 3 - Create the basic application functions

In the same directory as *``__init__.py``*, you need the following functions in a file named 'main.py' or ``{app_module}.py`` if loading externally:

```python
def init():
    # perform any initialization required for application start
    pass

def finalize():
    # perform any cleanup. The python bindings for most resources
    # should handle cleanup themselves, so only resources
    # which you are manually managing need to be cleaned up here
    pass

def configure( configuration ):
    # configuration is a dict {} object which contains any ini
    # settings from application.ini
    pass

def tick():
    # this function will be called every tick. This is where you
    # want your game logic to live
    pass

def render():
    # do rendering
    pass

```

##Step 4 - Point application.ini at your module

In the client/ directory, there is a file named application.ini. Below is an example configuration

```
[APPLICATION]
name=name_of_my_application
controller_enabled=[true/false]
telnet_enabled=[true/false]
telnet_host=[hostname e.g. 127.0.0.1]
telnet_port=[port e.g. 8888]
[my_application]
;any variables asigned here are passed into the "configure" method
;in your apps main.py 
```

###Generated application ini

If the file generated_application.ini is in the client/ directory, it will ignore the main application.ini. This is useful if you have a seperate launcher app that switches between multiple apps or orchestrates runtime settings.

#Step 5 - Launch the runtime

In the root of the repo, several run scripts are provided, for example launch_windowed_960.bat The current binary name is dopey3.exe, and it is invoked with the following parameters:

```
bin\dopey3.exe [W] [H] [FS FLAG] [VIRTUAL FPS]

e.g.

;launch a 1920x1080, fullscreen, 60fps instance of the runtime

bin\dopey3.exe 1920 1080 1 60
```
