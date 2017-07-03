#include <SDL.h>
#include <GLXW/glxw.h>

#include "extended_video.h"
#include "log.h"
#include "../hwgfx/blend_control.h"
#include "../hwgfx/label.h"
#include "../hwgfx/rect.h"
#include "../hwgfx/shader.h"
#include "../hwgfx/context.h"
#include "../hwgfx/text.h"
#include "../hwgfx/primitive.h"

gfx_texture                     _ui;
gfx_coordinate_uv_primitive     _screen_primitive;
gfx_shader                      _screen_shader;
unsigned int                    _extensions_loaded = 0;

void gfx_surface_render( SDL_Surface* img) {
    blend_enter                 ( BLENDMODE_OVER        ); 
    shader_bind                 (&_screen_shader        );
    texture_from_SDL_surface    (&_ui,      img         );
    texture_bind                (&_ui,      TEX_UNIT_0  );
    primitive_render            ((gfx_coordinate_primitive*)
                                 (&_screen_primitive)     );
    blend_exit                  (                       );
}


void initExtendedVideo() {
    root_gfx_size rgs;   

    if( _extensions_loaded == 0 ) {
        glxwInit();
        _extensions_loaded = 1;
    }


    log_message( CTT2_RT_MODULE_OPENGL, LOG_LEVEL_INFO, "..theoretically loaded GL extensions" );



    initRects();
    //initLabels();
    initText();

    rgs = gfx_get_root_gfx_size();


    texture_generate                    (&_ui, rgs.w, rgs.h);

    primitive_create_screen_primitive   ((void*)&_screen_primitive);

    shader_load                         (&_screen_shader, "shaders/hwgfx/screen.vert.glsl",
                                                          "shaders/hwgfx/texture.frag.glsl");

    log_message( CTT2_RT_MODULE_OPENGL, LOG_LEVEL_INFO, "..theoretically loaded shaders. May not have compiled");
}

void dropExtendedVideo() {
    primitive_destroy_coordinate_primitive
                                        (&_screen_primitive);
    shader_drop                         (&_screen_shader);
    texture_drop                        (&_ui);

    dropText();
    //dropLabels();
    dropRects();
}

void resizeExtendedVideo() {
    root_gfx_size rgs = gfx_get_root_gfx_size();

    texture_generate                    (&_ui, rgs.w, rgs.h);
}
