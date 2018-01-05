#include "cpp_context.h"
#include <cstdio>
#include "../../../hwgfx/misc.h"

using namespace std;

namespace bgl {
    namespace context {
        void clear( float r, float g, float b, float a) {
            gfx_set_clear_color(r,g,b,a);
            gfx_clear();
        }
    }
}
