#ifndef BGLPP_API_HWGFX_VIEW
#define BGLPP_API_HWGFX_VIEW

#include <utility>

namespace bgl{

    namespace axis {
        static const double Y_AXIS_UP = 1.0;
        static const double Y_AXIS_DOWN = -1.0;
	};

    class view {
        public:
            view( float x, float y );
            void set_reduction( float x, float y );
            static view centered_view( float width, float height, float y_axis );
            std::pair<float, float> getReduction();
        private:
            std::pair<float,float> reduction;
	};
};

#endif
