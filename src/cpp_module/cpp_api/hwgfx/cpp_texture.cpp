#include "cpp_texture.h"
#include <SDL_image.h>
#include <cstdio>
namespace bgl {

extern "C" void GXC_WAIT_FLUSH();

unsigned int texture::next_id = 0;

texture::texture() {
    exit(0);
}

texture::texture( const char* path, bool filtered ) {
    SDL_Surface* image = IMG_Load(path);
    #ifdef BEAGLE_CPPGFX_BACKEND_HWGFX
    _tex = static_cast<gfx_texture*>(malloc(sizeof(gfx_texture)));
    texture_generate_filtered(_tex, image->w, image->h);
    texture_from_SDL_surface( _tex, image );
    #endif

    id = next_id++;

    printf("GENERATED_ID %d\n", id);
}

#ifdef BEAGLE_CPPGFX_BACKEND_HWGFX
texture::texture( gfx_texture* cstyle_texture) {
    _tex = cstyle_texture;
    printf("_tex id %d\n", _tex->texture_id );
}
#endif

unsigned int texture::get_id() {
    #ifdef BEAGLE_CPPGFX_BACKEND_HWGFX
    return 0;
    #else
    return id;
    #endif
}

void texture::destroy() {
    #ifdef BEAGLE_CPPGFX_BACKEND_HWGFX
    texture_drop(_tex);
    #endif
}

void texture::bind_to_unit( unsigned int unit ) {
    #ifdef BEAGLE_CPPGFX_BACKEND_HWGFX
    texture_bind( _tex, unit );
    #endif
}

#ifdef BEAGLE_CPPGFX_BACKEND_HWGFX
gfx_texture* texture::getHwgfxTex() {
    return _tex;
}
#endif



}
