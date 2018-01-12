#include "cpp_channel_primitive.h"

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

    #ifdef BEAGLE_CPPGFX_BACKEND_HWGFX
    void channel_primitive::create_hwgfx_primitive(float** data, unsigned int len) {
        hwgfx_primitive = static_cast<gfx_channel_primitive*>(malloc(sizeof(gfx_channel_primitive)));

		primitive_create_channel_primitive(hwgfx_primitive, static_cast<int>(spec.size()), data, &spec[0], len);

    }

    void channel_primitive::update_hwgfx_primitive(float** data, unsigned int len) {
		primitive_update_channel_primitive(hwgfx_primitive, data, &spec[0], len);
		
    }

    void channel_primitive::destroy_hwgfx_primitive() {

        primitive_destroy_channel_primitive(hwgfx_primitive);

    }
    #endif

}
}
