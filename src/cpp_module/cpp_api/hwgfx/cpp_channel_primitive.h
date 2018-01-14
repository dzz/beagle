#ifndef BGLPP_API_HWGFX_CHANNEL_PRIMITIVE_H
#define BGLPP_API_HWGFX_CHANNEL_PRIMITIVE_H

#include "./cpp_hwgfx_config.h"
#ifdef BEAGLE_CPPGFX_BACKEND_HWGFX
#include "../../../hwgfx/primitive.h"
#endif

#include <vector>

namespace bgl {
    namespace primitive {

        class channel_primitive {
            public:
                channel_primitive(std::vector<int> && spec); 
                void prepare( float** data, unsigned int len);
                void render();
                void destroy();
            private:
                std::vector<int> spec;
                #ifdef BEAGLE_CPPGFX_BACKEND_HWGFX
                gfx_channel_primitive* hwgfx_primitive;
                void create_hwgfx_primitive( float** data, unsigned int len );
                void update_hwgfx_primitive( float** data, unsigned int len );
                void destroy_hwgfx_primitive();
                #endif
        };
    }
}

#endif
