import hwgfx
from client.gfx.framebuffer import externally_managed_render_target
from client.gfx.blend import externally_managed_blendmode

class lightmapper():

    def encode_lines( native_lines ):
        return list(chain(*chain(*geometry)))

    def encode_lights( native_lights):
        encoded = []
        for light in native_lights:
            encoded_light = [
                light['position'][0],
                light['position'][1],
                light['color'][0],
                light['color'][1],
                light['color'][2],
                light['radius'],
                -1 ]
            encoded.extend(encoded_light)
        return encoded

    def render_scene( lines, lights, camera, framebuffer ):
        cpos = camera.translate_position([0.0,0.0])

        if framebuffer:
            fb_ptr = framebuffer._fb
            fb_w = framebuffer._tex.w
            fb_h = framebuffer._tex.h
        else:
            fb_ptr = 0
            fb_w = 0
            fb_h = 0


        with externally_managed_render_target():
            with externally_managed_blendmode():
                hwgfx.pointlight_render_scene( 
                    lines,
                    lightmapper.encode_lights(lights),
                    camera.view[0],
                    camera.view[1],
                    cpos[0],
                    cpos[1],
                    camera.get_uniform_scale(),
                    fb_ptr,
                    fb_w,
                    fb_h 
                    )
            
            

