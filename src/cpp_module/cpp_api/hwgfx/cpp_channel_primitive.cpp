#include "cpp_channel_primitive.h"
#include <iostream>

#ifdef BEAGLE_CPPGFX_BACKEND_HWGFX
#include <stdlib.h>
#include <cstring>
#endif

namespace bgl {
namespace primitive {                

    channel_primitive::channel_primitive(std::vector<int> && spec):spec(spec) 
    #ifdef BEAGLE_CPPGFX_BACKEND_HWGFX
    , hwgfx_primitive(nullptr)
    #endif
    {
    }

    channel_primitive::~channel_primitive() {
        if(hwgfx_primitive == nullptr)
        destroy_hwgfx_primitive();
    }

    void channel_primitive::prepare( float** data, unsigned int len ) {
        #ifdef BEAGLE_CPPGFX_BACKEND_HWGFX
        if(hwgfx_primitive == nullptr) {
            create_hwgfx_primitive(data, len);    
        } else {
            update_hwgfx_primitive(data, len);
        }
        #endif
    }

    void channel_primitive::render() {
    #ifdef BEAGLE_CPPGFX_BACKEND_HWGFX
        primitive_render( static_cast<void*>(hwgfx_primitive));
    #endif
    }

#ifdef BEAGLE_CPPGFX_BACKEND_HWGFX


    typedef float gfx_float;
    void channel_primitive::create_hwgfx_primitive(float** data, unsigned int len) {
        hwgfx_primitive = static_cast<gfx_channel_primitive*>(malloc(sizeof(gfx_channel_primitive)));

        float** marshalled_data = new float*[spec.size()];
        int* clens = new int[spec.size()];

        for(unsigned int i=0; i<spec.size(); ++i) {
            marshalled_data[i] = new float[len*spec[i]];
            std::memcpy( marshalled_data[i], data[i], len*spec[i]*sizeof(float));
        }
        std::memcpy(clens, &spec[0], spec.size()*sizeof(int));
        primitive_create_channel_primitive(hwgfx_primitive, static_cast<int>(spec.size()), marshalled_data, clens, len);
    }

    void channel_primitive::update_hwgfx_primitive(float** data, unsigned int len) {
        float** marshalled_data = new float*[spec.size()];
        int* clens = new int[spec.size()];

        for(unsigned int i=0; i<spec.size(); ++i) {
            marshalled_data[i] = new float[len*spec[i]];
            std::memcpy( marshalled_data[i], data[i], len*spec[i]*sizeof(float));
        }
        std::memcpy(clens, &spec[0], spec.size()*sizeof(int));
        primitive_update_channel_primitive(hwgfx_primitive, data, &spec[0], len);
    }

    void channel_primitive::destroy_hwgfx_primitive() {
        primitive_destroy_channel_primitive(hwgfx_primitive);
    }
#endif

}
}
