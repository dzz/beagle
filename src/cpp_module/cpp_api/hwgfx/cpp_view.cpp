#include "cpp_view.h"

namespace bgl {

view::view(float x, float y) : reduction(x, y) { }

view view::centered_view(float width, float height, float y_axis) {
    return view(1.0f / (width / 2.0f), y_axis / (height / 2.0f));
}

std::pair<float, float> view::getReduction() {
    return reduction;
}

void view::set_reduction( float x, float y ) {
    reduction.first = x;
    reduction.second = y;
}

}
