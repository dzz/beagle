import localgfx
import client.system.log as log

class local_image:
    def __init__(self,img):
        self._img           = img
        [ self.w, self.h ]  = localgfx.img_dims(self._img)
        log.write( log.DEBUG, "Acquired image:{0} with dims [{1},{2}]".format(self._img, self.w, self.h))
        
    def __del__(self):
        localgfx.img_drop( self._img )
        log.write( log.DEBUG, "Dropped image:{0}".format(self._img))

    @classmethod
    def from_file(cls, filename):
        img         = localgfx.img_load(filename)
        return cls(img)

    @classmethod
    def from_dims(cls, w,h):
        img         = localgfx.img_create(w,h)
        return cls(img)

