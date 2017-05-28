import os
import os.path
import hwgfx
import client.beagle.beagle_environment  as beagle_environment
from client.gfx.texture import texture
from client.gfx.local_image import local_image

class tileset:
    def __init__(self, configuration, img_path = "", filtered = False ):

        # tiled will write out tilemap defs with
        # a FQ path, or path relative to the TEX file,
        # so, remap the filename to a directory passed
        # in explicitly which itself is a subdir of the application
        # container directory

        tail,head = os.path.split( configuration["image"] )
        self.channel_textures = {}
        self.image = beagle_environment.get_config("app_dir") + img_path + head
        self.imageheight = configuration["imageheight"]
        self.imagewidth = configuration["imagewidth"]
        self.margin = configuration["margin"]
        self.spacing = configuration["spacing"]

        if "extra_channels" in configuration:
            self.extra_channels = configuration["extra_channels"]
        else:
            self.extra_channels = []

        if "properties" in configuration:
            self.properties = configuration["properties"]
        else: 
            self.properties = {}
        self.firstgid = configuration["firstgid"]
        self.tileheight = configuration["tileheight"]
        self.tileproperties = configuration.get("tileproperties")
        if self.tileproperties is None:
            self.tileproperties = {}

        self.gids = []
        self.gidproperties = []
        self.gidcount=0
        self.gidrange = None
        self.texture = None
        self.compile()

    def compile(self, filtered = False ):
        self.texture     = texture.from_local_image( local_image.from_file(self.image), filtered  )
        for channel in self.extra_channels:
            fname, ext = os.path.splitext(self.image)
            channel_image = "{0}_{1}{2}".format( fname, channel, ext)
            self.channel_textures[channel] = texture.from_local_image( local_image.from_file(channel_image), filtered )
        uPix = self.margin
        vPix = self.margin
        tH = self.tileheight
        tS = self.spacing
        while(vPix<self.imageheight):
            while(uPix<self.imagewidth):
                self.gids.append( [uPix, vPix, tH, tH, {} ] )
                self.gidproperties.append({})
                uPix+= tS
                uPix+= tH
            uPix = 0
            vPix += tH
            vPix += tS
        #normalize gids
        for gid in self.gids:
            gid[0]/=self.imagewidth
            gid[2]/=self.imagewidth
            gid[1]/=self.imageheight
            gid[3]/=self.imageheight

        self.gidcount = len(self.gids)
        for key in self.tileproperties:
            ikey = int(key)
            self.gidproperties[ikey] = self.tileproperties[key]

    def get_gid(self, gid ):
        gid = gid - self.firstgid;
        if (gid<0) or (gid>self.gidcount):
            return False
        return self.gids[gid]

    def get_gid_no_check(self, gid ):
        gid = gid - self.firstgid;
        return self.gids[gid]


    def tile_prop(gid,key):
        return self.gidproperties[gid].get(key)
