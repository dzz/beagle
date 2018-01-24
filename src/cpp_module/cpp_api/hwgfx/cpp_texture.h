#ifndef BGLPP_API_HWGFX_TEXTURE_H
#define BGLPP_API_HWGFX_TEXTURE_H

#include "./cpp_hwgfx_config.h"
#ifdef BEAGLE_CPPGFX_BACKEND_HWGFX
#include "../../../hwgfx/texture.h"
#endif

namespace bgl { 

class texture {
    public:
        texture();
        texture( const char* path, bool filtered );
        #ifdef BEAGLE_CPPGFX_BACKEND_HWGFX
        texture( gfx_texture* cstyle_texture );
        #endif

        void set_view( float vx, float vy );
        void bind_to_unit( unsigned int unit );
        unsigned int get_id();
        void destroy();
        #ifdef BEAGLE_CPPGFX_BACKEND_HWGFX
        gfx_texture* getHwgfxTex();
        #endif
    private:
        unsigned int id;
        static unsigned int next_id;
        #ifdef BEAGLE_CPPGFX_BACKEND_HWGFX
        gfx_texture* _tex;
        #endif
};

}
#endif
