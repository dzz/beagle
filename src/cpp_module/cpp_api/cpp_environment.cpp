#include "cpp_environment.h"

extern "C" unsigned int host_get_screen_width();
extern "C" unsigned int host_get_screen_height();

namespace bgl {
namespace environment {

std::pair<unsigned int, unsigned int> get_screen_size() {
    return std::pair<unsigned int, unsigned int>(host_get_screen_width(), host_get_screen_height() );
}

}
}
