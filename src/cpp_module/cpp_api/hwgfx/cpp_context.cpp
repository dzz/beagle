#include "cpp_context.h"
#include <cstdio>
#include "./cpp_hwgfx_config.h"
#ifdef BEAGLE_CPPGFX_BACKEND_HWGFX
#include "../../../hwgfx/misc.h"
#endif

using namespace std;

namespace bgl {
    namespace context {
        void clear( float r, float g, float b, float a) {
            #ifdef BEAGLE_CPPGFX_BACKEND_HWGFX
            gfx_set_clear_color(r,g,b,a);
            gfx_clear();
            #endif
        }
    }
}
