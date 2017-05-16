# local_image

```python
from client.gfx.local_image import local_image
```

A local_image represents a bitmap in system (non-gpu) memory. Local images are typically used to load pngs, etc. for use on the GPU as a texture. It is also possible to render images on the GPU and then 'download' them back into system memory for saving to disc etc.

# usage

```python
from client.gfx.local_image import local_image

my_image = local_image.from_file("dogs.png")
blank_image = local_image.from_dims( 1920,1080)
```
