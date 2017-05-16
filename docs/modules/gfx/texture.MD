# texture

```python
    from client.gfx.texture import texture
``` 

The texture object is an abstraction of a texture resource on the graphics card. Typically, you will first create a local_image (which represents an image in system memory) and then upload that to a new texture object.

A texture object must be *bound* to a texture unit before being used. The default shaders will use texture unit 0, but if you are writing your own shaders, you can use up to 4 texture units as input. Many graphics cards support more than 4 texture units, and in the future CTT2 should allow you to address as many as the system provides.

Binding operations are expensive, so you should try and do as much drawing per-bind as possible. CTT2 will self manage this to a certain extent, by ignoring bind requests if the texture is already bound.

##Creating textures:

```python
from client.gfx.local_image import local_image
from client.gfx.texture import texture
from client.gfx.texture import units as texture_units

#create a 128x128 smoothed texture
tex = texture.from_dims( 128,128, filtered = True)
#create a 128x128 texture with no smoothing
tex = texture.from_dims( 128,128, filtered = False)
#create a texture from a local image
img = local_image.from_file("test.png")
tex = texture.from_local_image(img, filtered = True)    

#bind a texture for using in drawing
tex.bind(texture_units[0])
#any subsequent draws which reference texture unit 0 will now use "tex"


#download contents of a texture to a local image (for example,
#if you were rendering to a texture and now want to bring it 
#into system memory to save to disk)
tex.download_local_image( img )
#reupload a local image to a texture
tex.upload_local_image( img )
```

##Rendering to Textures

It is possible to render to a texture instead of the screen. Read about ```framebuffer``` objects for further information.

