#ifndef BGLPP_API_HWGFX_CHANNEL_PRIMITIVE_H
#define BGLPP_API_HWGFX_CHANNEL_PRIMITIVE_H

#include "../../../hwgfx/primitive.h"

namespace bgl {
    namespace primitive {
        class channel_primitive {

            public:
                channel_primitive();
                ~channel_primitive();
            private:
                gfx_channel_primitive* _primitive;
                
        }
    }
}

#endif
