import hwgfx
import client.gfx.shaders as shaders
import client.gfx.texture as texture

def rect_solid(r,color):
    shader = shaders.get(  "hwgfx/rect", 
                           "hwgfx/rect_solid" )

    shader.bind         ( [( "color", color )] )
    hwgfx.rect_draw( r[0],r[1],r[2],r[3] )

def rect_vgrad(r,color0,color1):
    shader = shaders.get(  "hwgfx/rect", 
                           "hwgfx/rect_vgrad" )

    shader.bind         ([ ( "color0", color0 ),
                           ( "color1", color1 ) ])
    hwgfx.rect_draw( r[0],r[1],r[2],r[3] )


pending_rect_draws = []

def rect_tile_start(tileset):
        rect_tile_flush_pending(tileset)

        shader = shaders.get(  "hwgfx/rect_tile", 
                               "hwgfx/rect_text" )

        tileset.texture.bind(texture.units[0])
        shader.bind([])


def rect_tile_end(tileset):
        rect_tile_flush_pending(tileset)

def rect_tile_flush_pending(tileset):
        global pending_rect_draws
        if len(pending_rect_draws) > 0:
            hwgfx.rect_draw_tex_array( pending_rect_draws )
        pending_rect_draws = []

def rect_tile_change_tileset(tileset):
        rect_tile_flush_pending(tileset);
        tileset.texture.bind(texture.units[0])


def rect_tile_raw(tileset, gid, x,y, scale = 1):
        if ( (x< (0-tileset.tileheight*scale)) or (y<(0-tileset.tileheight*scale)) or (x>1920) or (y>1080) ):
            return

        tile = tileset.get_gid(gid)
        #hwgfx.rect_draw_tex( 
        #                 x,
        #                 y,
        #                 tileset.tileheight*scale,
        #                 tileset.tileheight*scale,
        #                 tile[0],
        #                 tile[1],
        #                 tile[2], 
        #                 tile[3] )

        pending_rect_draws.append( [
                         int(x),
                         int(y),
                         int(tileset.tileheight*scale),
                         int(tileset.tileheight*scale),
                         float(tile[0]),
                         float(tile[1]),
                         float(tile[2]), 
                         float(tile[3]) ] )


def rect_tile(tileset, gid, x,y, scale = 1):
    tile = tileset.get_gid(gid)
    if(tile):
        shader = shaders.get(  "hwgfx/rect_tile", 
                               "hwgfx/rect_text" )

        shader.bind         ( [( "color0", [1.0,1.0,1.0,0.0] )] )
        shader.bind         ( [( "color1", [1.0,1.0,1.0,0.0] )] )

        tileset.texture.bind(texture.units[0])

        hwgfx.rect_draw_tex_array( [[
                         int(x),
                         int(y),
                         int(tileset.tileheight*scale),
                         int(tileset.tileheight*scale),
                         float(tile[0]),
                         float(tile[1]),
                         float(tile[2]), 
                         float(tile[3]) ]] )



def rect_brush(brush, view):

    if(brush.texture is None):
        return

    shader = shaders.get(  "hwgfx/rect_brush", 
                           "hwgfx/rect_text" )

    shader.bind         ( [( "color0", brush.colour )] )
    shader.bind         ( [( "color1", [1.0,1.0,1.0,0.5] )] )


    brush.texture.bind( texture.units[0] )


    pix_x = brush.r[0] - int(view.cam[0])
    pix_y = brush.r[1] - int(view.cam[1])
    
    par_x = ((pix_x - view.screen_center[0] + (brush.r[2]/2)) * brush.parallax) + view.screen_center[0] - (brush.r[2]/2)
    par_y = ((pix_y - view.screen_center[1] + (brush.r[3]/2)) * brush.parallax) + view.screen_center[1] - (brush.r[3]/2)

    dif_x = pix_x - par_x
    dif_y = pix_y - par_y

    dif_x /= brush.texture.w
    dif_y /= brush.texture.h

    hwgfx.rect_draw_tex( 
                         int(par_x),
                         int(par_y),
                         brush.r[2],
                         brush.r[3],
                         brush.uvs[0],# + dif_x, 
                         brush.uvs[1],# + dif_y, 
                         brush.uvs[2], 
                         brush.uvs[3] )
