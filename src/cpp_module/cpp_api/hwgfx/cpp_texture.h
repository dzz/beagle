#ifndef BGLPP_API_HWGFX_TEXTURE_H
#define BGLPP_API_HWGFX_TEXTURE_H

#include "./cpp_hwgfx_config.h"
#ifdef BEAGLE_CPPGFX_BACKEND_HWGFX
#include "../../../hwgfx/texture.h"
#endif

namespace bgl { 

class texture {
    public:
        texture( const char* path, bool filtered );
        void bind_to_unit( unsigned int unit );
        void destroy();
        #ifdef BEAGLE_CPPGFX_BACKEND_HWGFX
        gfx_texture* getHwgfxTex();
        #endif
    private:
        #ifdef BEAGLE_CPPGFX_BACKEND_HWGFX
        gfx_texture* _tex;
        #endif
};

}
#endif
