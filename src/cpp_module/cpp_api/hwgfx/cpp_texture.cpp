#include "cpp_texture.h"
#include <SDL_image.h>
#include <cstdio>
namespace bgl {

extern "C" void GXC_WAIT_FLUSH();

texture::texture( const char* path, bool filtered ) {
    SDL_Surface* image = IMG_Load(path);
	
    printf("LOADED IMAGE %s!\n",path);
    #ifdef BEAGLE_CPPGFX_BACKEND_HWGFX
    _tex = static_cast<gfx_texture*>(malloc(sizeof(gfx_texture)));
    texture_generate_filtered(_tex, image->w, image->h);
    GXC_WAIT_FLUSH();

    printf("%d width\n", image->w);
    texture_from_SDL_surface( _tex, image );

    printf("SENT COMMANDS\n");
    GXC_WAIT_FLUSH();
    printf("BUFFER FLUSHED\n");

	printf("%d texture id\n", _tex->texture_id);


    #endif
}

void texture::bind_to_unit( unsigned int unit ) {
    #ifdef BEAGLE_CPPGFX_BACKEND_HWGFX
    GXC_WAIT_FLUSH();
    texture_bind( _tex, unit );
    GXC_WAIT_FLUSH();
    #endif
}

#ifdef BEAGLE_CPPGFX_BACKEND_HWGFX
gfx_texture* texture::getHwgfxTex() {
    return _tex;
}
#endif

texture::~texture() {
    #ifdef BEAGLE_CPPGFX_BACKEND_HWGFX
    GXC_WAIT_FLUSH();
    texture_drop(_tex);
    GXC_WAIT_FLUSH();
    #endif
}


}
