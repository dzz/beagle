# host_config

```python
import client.ctt2.host_config  as host_config

get_config(key)
set_config(key,value)
```

host_config is a container for settings and variables. 

#Status Rendering

```python
host_config.set_config("render_status",True)
host_config.set_config("render_status",False)
```

If render_status is True, the application host will render the application status string in the top left. For debugging only.

#Application Configuration

```python
host_config.get_config("app_name")
host_config.get_config("app_dir")
```

These values are set when the application host loads the application. When loading application resources, such as images,textures, tilesets, etc. they should be loaded relative to app_dir.

```python
host_config.get_config("screen_dims")
```

The screen_dims setting contains an array in the form of [ w, h ] which contains the dimensions of the current render target, irrespective of any viewports that have been configured.
