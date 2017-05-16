from client.gfx.primitive import primitive,draw_mode
import client.gfx.shaders as shaders



class sprite():
    def __init__(self, sprite_renderer, named_animations = { "default" : [0] } , current_animation = "default", ticks_per_frame = 2, size = None, ):
        self.named_animations = named_animations
        self.frame_index = 0
        self.ticks = 0
        self.current_animation = current_animation
        self.ticks_per_frame = ticks_per_frame
        self.primitives = None

        self.sprite_renderer = sprite_renderer
        self.size = size
        self.animating = True

        if self.size is None:
            self.size = self.sprite_renderer.tileset.tileheight

        self.compile()

    def compile(self):
        self.primitives = {}
        for animation in self.named_animations:
            self.primitives[animation] = []
            for frame in self.named_animations[animation]:
                tile = list( self.sprite_renderer.tileset.get_gid( frame ) )
                if not tile:
                    continue
                sz = self.size
                tile[2]-=0.001;
                tile[3]-=0.001;
                self.primitives[animation].append( primitive( draw_mode.TRIS, [ 
                                                                     [ 0.0,    0.0,  ],
                                                                     [ 0.0+sz, 0.0,  ],
                                                                     [ 0.0+sz, 0.0+sz],
                                                                     [ 0.0+sz, 0.0+sz],
                                                                     [ 0.0,    0.0+sz],
                                                                     [ 0.0,    0.0   ]  ],
                                                           [
                                                            [ tile[0],         tile[1]         ],
                                                            [ tile[0]+tile[2], tile[1]         ],
                                                            [ tile[0]+tile[2], tile[1]+tile[3] ],
                                                            [ tile[0]+tile[2], tile[1]+tile[3] ],
                                                            [ tile[0]        , tile[1]+tile[3] ],
                                                            [ tile[0],         tile[1]         ] ]) )

    def set_animation_enabled(self,enabled):
        self.animating = enabled

    def select_animation( self, key ):
        self.current_animation = key
        self.frame_index = 0

    def tick(self):
        if not self.animating:
            return
        self.ticks = (self.ticks + 1) % (self.ticks_per_frame)
        if(self.ticks == 0 ):
            self.frame_index = (self.frame_index + 1) % len( self.named_animations[self.current_animation] )

    def get_current_primitive(self):
        return self.primitives[self.current_animation][self.frame_index]
        

class sprite_renderer():
    def __init__( self, tileset, coordinates = [1.0,1.0],):
        self.tileset = tileset
        self.coordinates = coordinates
        self.shader = shaders.get_unique( "hwgfx/sprite", "hwgfx/tilemap" )

    def render(self, sprite_render_operations):
        self.tileset.texture.bind(0)
        self.shader.bind([ ("view", self.coordinates) ])
        for sprite_render_operation in sprite_render_operations:
            if(len(sprite_render_operation)==3):
                sprite = sprite_render_operation[0]
                translation_local = sprite_render_operation[1]
                scale = sprite_render_operation[2]
                self.shader.bind([ ("scale_local", [scale]), ("translation_local", translation_local ), ("scale_world",[scale_world] ),("translation_world",translation_world), ("rotation_local", [rotation_local]) ])
                sprite.get_current_primitive().render()
            elif(len(sprite_render_operation)==5):
                sprite = sprite_render_operation[0]
                translation_local = sprite_render_operation[1]
                scale = sprite_render_operation[2]
                translation_world = sprite_render_operation[3]
                scale_world = sprite_render_operation[4]
                rotation_local = 0.0
                self.shader.bind([ ("scale_local", [scale]), ("translation_local", translation_local ), ("scale_world",[scale_world] ),("translation_world",translation_world), ("rotation_local", [rotation_local]) ])
                sprite.get_current_primitive().render()
            elif(len(sprite_render_operation)==6):
                sprite = sprite_render_operation[0]
                translation_local = sprite_render_operation[1]
                scale = sprite_render_operation[2]
                rotation_local = sprite_render_operation[3]
                translation_world = sprite_render_operation[4]
                scale_world = sprite_render_operation[5]
                self.shader.bind([ ("scale_local", [scale]), ("translation_local", translation_local ), ("scale_world",[scale_world] ),("translation_world",translation_world), ("rotation_local", [rotation_local]) ])
                sprite.get_current_primitive().render()


